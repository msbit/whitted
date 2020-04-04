#include <cmath>
#include <utility>

#include "sphere.h"

bool solveQuadratic(const float &a, const float &b, const float &c, float &x0,
                    float &x1) {
  const float discr = b * b - 4 * a * c;
  if (discr < 0) {
    return false;
  }

  if (discr == 0) {
    x0 = x1 = -0.5 * b / a;
  } else {
    const float q =
        (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
    x0 = q / a;
    x1 = c / q;
  }
  if (x0 > x1) {
    std::swap(x0, x1);
  }
  return true;
}

Sphere::Sphere(const Vec3f &center, const float &radius)
    : center(center), radius(radius), radius2(radius * radius) {}

bool Sphere::intersect(const Vec3f &orig, const Vec3f &dir, float &tnear,
                       uint32_t &index, Vec2f &uv) const {
  // analytic solution
  const Vec3f L = orig - center;
  const float a = Vec3f::dotProduct(dir, dir);
  const float b = 2 * Vec3f::dotProduct(dir, L);
  const float c = Vec3f::dotProduct(L, L) - radius2;
  float t0;
  float t1;
  if (!solveQuadratic(a, b, c, t0, t1)) {
    return false;
  }

  if (t0 < 0) {
    t0 = t1;
  }
  if (t0 < 0) {
    return false;
  }

  tnear = t0;

  return true;
}

void Sphere::getSurfaceProperties(const Vec3f &P, const Vec3f &I,
                                  const uint32_t &index, const Vec2f &uv,
                                  Vec3f &N, Vec2f &st) const {
  N = Vec3f::normalize(P - center);
}
