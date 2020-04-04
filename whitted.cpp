//[header]
// A simple program to demonstrate how to implement Whitted-style ray-tracing
//[/header]
//[compile]
// Download the whitted.cpp file to a folder.
// Open a shell/terminal, and run the following command where the files is
// saved:
//
// c++ -o whitted whitted.cpp -std=c++11 -O3
//
// Run with: ./whitted. Open the file ./out.png in Photoshop or any program
// reading PPM files.
//[/compile]
//[ignore]
// Copyright (C) 2012  www.scratchapixel.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//[/ignore]

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

float clamp(const float &low, const float &high, const float &value) {
  return std::max(low, std::min(high, value));
}

float deg2rad(const float &deg) { return deg * M_PI / 180; }

struct Options {
  uint32_t width;
  uint32_t height;
  float fov;
  uint8_t maxDepth;
  Vec3f backgroundColor;
  float bias;
};

// [comment]
// Compute reflection direction
// [/comment]
Vec3f reflect(const Vec3f &I, const Vec3f &N) {
  return I - 2 * Vec3f::dotProduct(I, N) * N;
}

// [comment]
// Compute refraction direction using Snell's law
//
// We need to handle with care the two possible situations:
//
//    - When the ray is inside the object
//
//    - When the ray is outside.
//
// If the ray is outside, you need to make cosi positive cosi = -N.I
//
// If the ray is inside, you need to invert the refractive indices and negate
// the normal N
// [/comment]
Vec3f refract(const Vec3f &I, const Vec3f &N, const float &ior) {
  float cosI = clamp(-1, 1, Vec3f::dotProduct(I, N));
  float etaI = 1;
  float etaT = ior;
  Vec3f n = N;
  if (cosI < 0) {
    cosI = -cosI;
  } else {
    std::swap(etaI, etaT);
    n = -N;
  }
  const float eta = etaI / etaT;
  const float k = 1 - eta * eta * (1 - cosI * cosI);
  return k < 0 ? 0 : eta * I + (eta * cosI - sqrtf(k)) * n;
}

// [comment]
// Compute Fresnel equation
//
// \param I is the incident view direction
//
// \param N is the normal at the intersection point
//
// \param ior is the mateural refractive index
//
// \param[out] kr is the amount of light reflected
// [/comment]
void fresnel(const Vec3f &I, const Vec3f &N, const float &ior, float &kr) {
  float cosI = clamp(-1, 1, Vec3f::dotProduct(I, N));
  float etaI = 1;
  float etaT = ior;
  if (cosI > 0) {
    std::swap(etaI, etaT);
  }
  // Compute sini using Snell's law
  const float sinT = etaI / etaT * sqrtf(std::max(0.f, 1 - cosI * cosI));
  // Total internal reflection
  if (sinT >= 1) {
    kr = 1;
  } else {
    const float cosT = sqrtf(std::max(0.f, 1 - sinT * sinT));
    cosI = fabsf(cosI);
    const float Rs =
        ((etaT * cosI) - (etaI * cosT)) / ((etaT * cosI) + (etaI * cosT));
    const float Rp =
        ((etaI * cosI) - (etaT * cosT)) / ((etaI * cosI) + (etaT * cosT));
    kr = (Rs * Rs + Rp * Rp) / 2;
  }
  // As a consequence of the conservation of energy, transmittance is given by:
  // kt = 1 - kr;
}

// [comment]
// Returns true if the ray intersects an object, false otherwise.
//
// \param origin is the ray origin
//
// \param direction is the ray direction
//
// \param objects is the list of objects the scene contains
//
// \param[out] tNear contains the distance to the cloesest intersected object.
//
// \param[out] index stores the index of the intersect triangle if the
// interesected object is a mesh.
//
// \param[out] uv stores the u and v barycentric coordinates of the intersected
// point
//
// \param[out] *hitObject stores the pointer to the intersected object (used to
// retrieve material information, etc.)
//
// \param isShadowRay is it a shadow ray. We can return from the function sooner
// as soon as we have found a hit.
// [/comment]
bool trace(const Vec3f &origin, const Vec3f &direction,
           const std::vector<std::unique_ptr<Object>> &objects, float &tNear,
           uint32_t &index, Vec2f &uv, Object **hitObject) {
  *hitObject = nullptr;
  for (const auto &object : objects) {
    float tNearK = kInfinity;
    uint32_t indexK;
    Vec2f uvK;
    if (object->intersect(origin, direction, tNearK, indexK, uvK) &&
        tNearK < tNear) {
      *hitObject = object.get();
      tNear = tNearK;
      index = indexK;
      uv = uvK;
    }
  }

  return (*hitObject != nullptr);
}

// [comment]
// Implementation of the Whitted-syle light transport algorithm (E [S*] (D|G) L)
//
// This function is the function that compute the color at the intersection
// point of a ray defined by a position and a direction. Note that thus function
// is recursive (it calls itself).
//
// If the material of the intersected object is either reflective or reflective
// and refractive, then we compute the reflection/refracton direction and cast
// two new rays into the scene by calling the castRay() function recursively.
// When the surface is transparent, we mix the reflection and refraction color
// using the result of the fresnel equations (it computes the amount of
// reflection and refractin depending on the surface normal, incident view
// direction and surface refractive index).
//
// If the surface is duffuse/glossy we use the Phong illumation model to compute
// the color at the intersection point.
// [/comment]
Vec3f castRay(const Vec3f &origin, const Vec3f &direction,
              const std::vector<std::unique_ptr<Object>> &objects,
              const std::vector<std::unique_ptr<Light>> &lights,
              const Options &options, uint32_t depth, bool test = false) {
  if (depth > options.maxDepth) {
    return options.backgroundColor;
  }

  Vec3f hitColor = options.backgroundColor;
  float tNear = kInfinity;
  Vec2f uv;
  uint32_t index = 0;
  Object *hitObject = nullptr;
  if (trace(origin, direction, objects, tNear, index, uv, &hitObject)) {
    const Vec3f hitPoint = origin + direction * tNear;
    Vec3f N;  // normal
    Vec2f st; // st coordinates
    hitObject->getSurfaceProperties(hitPoint, direction, index, uv, N, st);
    const Vec3f tmp = hitPoint;
    switch (hitObject->materialType) {
    case REFLECTION_AND_REFRACTION: {
      const Vec3f reflectionDirection = Vec3f::normalize(reflect(direction, N));
      const Vec3f refractionDirection =
          Vec3f::normalize(refract(direction, N, hitObject->ior));
      const Vec3f reflectionRayOrigin =
          (Vec3f::dotProduct(reflectionDirection, N) < 0)
              ? hitPoint - N * options.bias
              : hitPoint + N * options.bias;
      const Vec3f refractionRayOrigin =
          (Vec3f::dotProduct(refractionDirection, N) < 0)
              ? hitPoint - N * options.bias
              : hitPoint + N * options.bias;
      const Vec3f reflectionColor =
          castRay(reflectionRayOrigin, reflectionDirection, objects, lights,
                  options, depth + 1, true);
      const Vec3f refractionColor =
          castRay(refractionRayOrigin, refractionDirection, objects, lights,
                  options, depth + 1, true);
      float kr;
      fresnel(direction, N, hitObject->ior, kr);
      hitColor = reflectionColor * kr + refractionColor * (1 - kr);
      break;
    }
    case REFLECTION: {
      float kr;
      fresnel(direction, N, hitObject->ior, kr);
      const Vec3f reflectionDirection = reflect(direction, N);
      const Vec3f reflectionRayOrigin =
          (Vec3f::dotProduct(reflectionDirection, N) < 0)
              ? hitPoint + N * options.bias
              : hitPoint - N * options.bias;
      hitColor = castRay(reflectionRayOrigin, reflectionDirection, objects,
                         lights, options, depth + 1) *
                 kr;
      break;
    }
    default: {
      // [comment]
      // We use the Phong illumation model int the default case. The phong model
      // is composed of a diffuse and a specular reflection component.
      // [/comment]
      Vec3f lightAmt = 0;
      Vec3f specularColor = 0;
      const Vec3f shadowPointOrig = (Vec3f::dotProduct(direction, N) < 0)
                                        ? hitPoint + N * options.bias
                                        : hitPoint - N * options.bias;
      // [comment]
      // Loop over all lights in the scene and sum their contribution up
      // We also apply the lambert cosine law here though we haven't explained
      // yet what this means.
      // [/comment]
      for (const auto &light : lights) {
        Vec3f lightDir = light->position - hitPoint;
        // square of the distance between hitPoint and the light
        const float lightDistance2 = Vec3f::dotProduct(lightDir, lightDir);
        lightDir = Vec3f::normalize(lightDir);
        const float LdotN = std::max(0.f, Vec3f::dotProduct(lightDir, N));
        Object *shadowHitObject = nullptr;
        float tNearShadow = kInfinity;
        // is the point in shadow, and is the nearest occluding object closer to
        // the object than the light itself?
        const bool inShadow = trace(shadowPointOrig, lightDir, objects,
                                    tNearShadow, index, uv, &shadowHitObject) &&
                              tNearShadow * tNearShadow < lightDistance2;
        lightAmt += (1 - inShadow) * light->intensity * LdotN;
        const Vec3f reflectionDirection = reflect(-lightDir, N);
        specularColor +=
            powf(std::max(0.f,
                          -Vec3f::dotProduct(reflectionDirection, direction)),
                 hitObject->specularExponent) *
            light->intensity;
      }
      hitColor = lightAmt * hitObject->evalDiffuseColor(st) * hitObject->Kd +
                 specularColor * hitObject->Ks;
      break;
    }
    }
  }

  return hitColor;
}

// [comment]
// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
// [/comment]
void render(const Options &options,
            const std::vector<std::unique_ptr<Object>> &objects,
            const std::vector<std::unique_ptr<Light>> &lights) {
  Vec3f *framebuffer = new Vec3f[options.width * options.height];
  Vec3f *pix = framebuffer;
  const float scale = tan(deg2rad(options.fov * 0.5));
  const float imageAspectRatio = options.width / (float)options.height;
  const Vec3f origin(0);
  for (uint32_t j = 0; j < options.height; ++j) {
    for (uint32_t i = 0; i < options.width; ++i) {
      // generate primary ray direction
      const float x =
          (2 * (i + 0.5) / (float)options.width - 1) * imageAspectRatio * scale;
      const float y = (1 - 2 * (j + 0.5) / (float)options.height) * scale;
      const Vec3f direction = Vec3f::normalize({x, y, -1});
      *(pix++) = castRay(origin, direction, objects, lights, options, 0);
    }
  }

  // save framebuffer to file
  std::ofstream ofs;
  ofs.open("./out.ppm");
  ofs << "P6\n" << options.width << " " << options.height << "\n255\n";
  for (uint32_t i = 0; i < options.height * options.width; ++i) {
    const char r = (char)(255 * clamp(0, 1, framebuffer[i].x));
    const char g = (char)(255 * clamp(0, 1, framebuffer[i].y));
    const char b = (char)(255 * clamp(0, 1, framebuffer[i].z));
    ofs << r << g << b;
  }

  ofs.close();

  delete[] framebuffer;
}

// [comment]
// In the main function of the program, we create the scene (create objects and
// lights) as well as set the options for the render (image widht and height,
// maximum recursion depth, field-of-view, etc.). We then call the render
// function().
// [/comment]
int main(int argc, char **argv) {
  // creating the scene (adding objects and lights)
  std::vector<std::unique_ptr<Object>> objects;
  std::vector<std::unique_ptr<Light>> lights;

  Sphere *sphere1 = new Sphere({-1, 0, -12}, 2);
  sphere1->materialType = DIFFUSE_AND_GLOSSY;
  sphere1->diffuseColor = {0.6, 0.7, 0.8};
  Sphere *sphere2 = new Sphere({0.5, -0.5, -8}, 1.5);
  sphere2->ior = 1.5;
  sphere2->materialType = REFLECTION_AND_REFRACTION;

  objects.push_back(std::unique_ptr<Sphere>(sphere1));
  objects.push_back(std::unique_ptr<Sphere>(sphere2));

  const std::vector<Vec3f> verts = {
      {-5, -3, -6}, {5, -3, -6}, {5, -3, -16}, {-5, -3, -16}};
  const std::vector<uint32_t> vertIndex = {0, 1, 3, 1, 2, 3};
  const std::vector<Vec2f> st = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  MeshTriangle *mesh = new MeshTriangle(verts, vertIndex, 2, st);
  mesh->materialType = DIFFUSE_AND_GLOSSY;

  objects.push_back(std::unique_ptr<MeshTriangle>(mesh));

  lights.push_back(std::unique_ptr<Light>(new Light({-20, 70, 20}, 0.5)));
  lights.push_back(std::unique_ptr<Light>(new Light({30, 50, -12}, 1)));

  // setting up options
  const Options options = {1600,   1600, 90, 5, {0.235294, 0.67451, 0.843137},
                           0.00001};

  // finally, render
  render(options, objects, lights);

  return 0;
}
