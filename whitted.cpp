#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "light.h"
#include "mesh_triangle.h"
#include "object.h"
#include "sphere.h"
#include "vec2f.h"
#include "vec3f.h"

const float kInfinity = std::numeric_limits<float>::max();

auto deg2rad(float deg) -> float { return deg * M_PI / 180; }

struct Options {
  uint32_t width;
  uint32_t height;
  float fov;
  uint8_t maxDepth;
  Vec3f backgroundColor;
  float bias;
};

auto reflect(const Vec3f &I, const Vec3f &N) -> Vec3f {
  return I - 2 * Vec3f::dotProduct(I, N) * N;
}

auto refract(const Vec3f &I, const Vec3f &N, float ior) -> Vec3f {
  auto cosI = std::clamp(Vec3f::dotProduct(I, N), -1.f, 1.f);
  auto etaI = 1.f;
  auto etaT = ior;
  auto n = N;
  if (cosI < 0) {
    cosI = -cosI;
  } else {
    std::swap(etaI, etaT);
    n = -N;
  }
  const auto eta = etaI / etaT;
  const auto k = 1 - eta * eta * (1 - cosI * cosI);
  return k < 0 ? 0 : eta * I + (eta * cosI - std::sqrtf(k)) * n;
}

auto fresnel(const Vec3f &I, const Vec3f &N, float ior) -> float {
  auto cosI = std::clamp(Vec3f::dotProduct(I, N), -1.f, 1.f);
  auto etaI = 1.f;
  auto etaT = ior;
  if (cosI > 0) {
    std::swap(etaI, etaT);
  }
  // Compute sini using Snell's law
  const auto sinT = etaI / etaT * std::sqrtf(std::max(0.f, 1 - cosI * cosI));
  // Total internal reflection
  if (sinT >= 1) {
    return 1;
  }

  const auto cosT = std::sqrtf(std::max(0.f, 1 - sinT * sinT));
  cosI = std::fabsf(cosI);
  const auto Rs =
      ((etaT * cosI) - (etaI * cosT)) / ((etaT * cosI) + (etaI * cosT));
  const auto Rp =
      ((etaI * cosI) - (etaT * cosT)) / ((etaI * cosI) + (etaT * cosT));
  return (Rs * Rs + Rp * Rp) / 2;
  // As a consequence of the conservation of energy, transmittance is given by:
  // kt = 1 - kr;
}

auto trace(const Vec3f &origin, const Vec3f &direction,
           const std::vector<std::unique_ptr<Object>> &objects, float &tNear,
           uint32_t &index, Vec2f &uv, Object **hitObject) -> bool {
  *hitObject = nullptr;
  for (const auto &object : objects) {
    auto tNearK = kInfinity;
    uint32_t indexK;
    Vec2f uvK;
    if (!object->intersect(origin, direction, tNearK, indexK, uvK)) {
      continue;
    }
    if (tNearK >= tNear) {
      continue;
    }

    *hitObject = object.get();
    tNear = tNearK;
    index = indexK;
    uv = uvK;
  }

  return (*hitObject != nullptr);
}

auto castRay(const Vec3f &origin, const Vec3f &direction,
             const std::vector<std::unique_ptr<Object>> &objects,
             const std::vector<Light> &lights, const Options &options,
             uint32_t depth) -> Vec3f {
  if (depth > options.maxDepth) {
    return options.backgroundColor;
  }

  auto tNear = kInfinity;
  Vec2f uv;
  uint32_t index = 0;
  Object *hitObject = nullptr;
  if (!trace(origin, direction, objects, tNear, index, uv, &hitObject)) {
    return options.backgroundColor;
  }

  auto hitColor = options.backgroundColor;
  const auto hitPoint = origin + direction * tNear;
  auto properties =
      hitObject->surfaceProperties(hitPoint, direction, index, uv);
  switch (hitObject->materialType) {
  case REFLECTION_AND_REFRACTION: {
    const auto reflectionDirection =
        Vec3f::normalize(reflect(direction, properties.N));
    const auto refractionDirection =
        Vec3f::normalize(refract(direction, properties.N, hitObject->ior));
    const auto reflectionRayOrigin =
        (Vec3f::dotProduct(reflectionDirection, properties.N) < 0)
            ? hitPoint - properties.N * options.bias
            : hitPoint + properties.N * options.bias;
    const auto refractionRayOrigin =
        (Vec3f::dotProduct(refractionDirection, properties.N) < 0)
            ? hitPoint - properties.N * options.bias
            : hitPoint + properties.N * options.bias;
    const auto reflectionColor =
        castRay(reflectionRayOrigin, reflectionDirection, objects, lights,
                options, depth + 1);
    const auto refractionColor =
        castRay(refractionRayOrigin, refractionDirection, objects, lights,
                options, depth + 1);
    auto kr = fresnel(direction, properties.N, hitObject->ior);
    hitColor = reflectionColor * kr + refractionColor * (1 - kr);
    break;
  }
  case REFLECTION: {
    auto kr = fresnel(direction, properties.N, hitObject->ior);
    const auto reflectionDirection = reflect(direction, properties.N);
    const auto reflectionRayOrigin =
        (Vec3f::dotProduct(reflectionDirection, properties.N) < 0)
            ? hitPoint + properties.N * options.bias
            : hitPoint - properties.N * options.bias;
    hitColor = castRay(reflectionRayOrigin, reflectionDirection, objects,
                       lights, options, depth + 1) *
               kr;
    break;
  }
  case DIFFUSE_AND_GLOSSY:
  default: {
    // [comment]
    // We use the Phong illumation model int the default case. The phong model
    // is composed of a diffuse and a specular reflection component.
    // [/comment]
    Vec3f lightAmt = 0;
    Vec3f specularColor = 0;
    const auto shadowPointOrig =
        (Vec3f::dotProduct(direction, properties.N) < 0)
            ? hitPoint + properties.N * options.bias
            : hitPoint - properties.N * options.bias;
    // [comment]
    // Loop over all lights in the scene and sum their contribution up
    // We also apply the lambert cosine law here though we haven't explained
    // yet what this means.
    // [/comment]
    for (const auto &light : lights) {
      auto lightDir = light.position - hitPoint;
      // square of the distance between hitPoint and the light
      const auto lightDistance2 = Vec3f::dotProduct(lightDir, lightDir);
      lightDir = Vec3f::normalize(lightDir);
      const auto LdotN =
          std::max(0.f, Vec3f::dotProduct(lightDir, properties.N));
      Object *shadowHitObject = nullptr;
      auto tNearShadow = kInfinity;
      // is the point in shadow, and is the nearest occluding object closer to
      // the object than the light itself?
      const auto inShadow = trace(shadowPointOrig, lightDir, objects,
                                  tNearShadow, index, uv, &shadowHitObject) &&
                            tNearShadow * tNearShadow < lightDistance2;
      lightAmt += (1 - inShadow) * light.intensity * LdotN;
      const auto reflectionDirection = reflect(-lightDir, properties.N);
      specularColor +=
          powf(
              std::max(0.f, -Vec3f::dotProduct(reflectionDirection, direction)),
              hitObject->specularExponent) *
          light.intensity;
    }
    hitColor =
        lightAmt * hitObject->evalDiffuseColor(properties.st) * hitObject->Kd +
        specularColor * hitObject->Ks;
    break;
  }
  }

  return hitColor;
}

auto render(const Options &options,
            const std::vector<std::unique_ptr<Object>> &objects,
            const std::vector<Light> &lights) {
  auto buffer = new Vec3f[options.width * options.height];
  auto pixel = buffer;
  const auto scale = std::tan(deg2rad(options.fov * 0.5));
  const auto imageAspectRatio = options.width / (float)options.height;
  const Vec3f origin(0);
  for (auto j = 0; j < options.height; ++j) {
    for (auto i = 0; i < options.width; ++i) {
      // generate primary ray direction
      const float x =
          (2 * (i + 0.5) / (float)options.width - 1) * imageAspectRatio * scale;
      const float y = (1 - 2 * (j + 0.5) / (float)options.height) * scale;
      const auto direction = Vec3f::normalize({x, y, -1});
      *(pixel++) = castRay(origin, direction, objects, lights, options, 0);
    }
  }

  // save buffer to file
  std::ofstream ofs;
  ofs.open("./out.ppm");
  ofs << "P6\n" << options.width << " " << options.height << "\n255\n";
  for (auto i = 0; i < options.height * options.width; ++i) {
    ofs << (char)(255 * std::clamp(buffer[i].x, 0.f, 1.f));
    ofs << (char)(255 * std::clamp(buffer[i].y, 0.f, 1.f));
    ofs << (char)(255 * std::clamp(buffer[i].z, 0.f, 1.f));
  }

  ofs.close();

  delete[] buffer;
}

auto main(int argc, char **argv) -> int {
  // creating the scene (adding objects and lights)
  std::vector<std::unique_ptr<Object>> objects;
  std::vector<Light> lights;

  auto sphere1 = new Sphere({-1, 0, -12}, 2);
  sphere1->materialType = DIFFUSE_AND_GLOSSY;
  sphere1->diffuseColor = {0.6, 0.7, 0.8};
  auto sphere2 = new Sphere({0.5, -0.5, -8}, 1.5);
  sphere2->ior = 1.5;
  sphere2->materialType = REFLECTION_AND_REFRACTION;

  objects.emplace_back(sphere1);
  objects.emplace_back(sphere2);

  const std::vector<Vec3f> verts = {
      {-5, -3, -6}, {5, -3, -6}, {5, -3, -16}, {-5, -3, -16}};
  const std::vector<uint32_t> vertIndex = {0, 1, 3, 1, 2, 3};
  const std::vector<Vec2f> st = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  auto mesh = new MeshTriangle(verts, vertIndex, 2, st);
  mesh->materialType = DIFFUSE_AND_GLOSSY;

  objects.emplace_back(mesh);

  lights.emplace_back(Vec3f{-20, 70, 20}, 0.5);
  lights.emplace_back(Vec3f{30, 50, -12}, 1);

  // setting up options
  const Options options = {1600,   1600, 90, 5, {0.235294, 0.67451, 0.843137},
                           0.00001};

  // finally, render
  render(options, objects, lights);

  return 0;
}
