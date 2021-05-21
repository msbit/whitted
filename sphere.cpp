#include <cmath>
#include <optional>
#include <utility>

#include "sphere.h"

auto solveQuadratic(float a, float b, float c) -> std::optional<Vec2f> {
  const auto discr = b * b - 4 * a * c;
  if (discr < 0) {
    return std::nullopt;
  }

  float x0;
  float x1;

  if (discr == 0) {
    x0 = x1 = -0.5 * b / a;
  } else {
    const auto q =
        (b > 0) ? -0.5 * (b + std::sqrt(discr)) : -0.5 * (b - std::sqrt(discr));
    x0 = q / a;
    x1 = c / q;
  }
  if (x0 > x1) {
    std::swap(x0, x1);
  }
  return Vec2f{x0, x1};
}

Sphere::Sphere(const Vec3f c, float r) : center(c), radius(r), radius2(r * r) {}

auto Sphere::intersect(const Vec3f &origin, const Vec3f &direction,
                       float &tnear, uint32_t &, Vec2f &) const -> bool {
  // analytic solution
  const auto L = origin - center;
  const auto a = Vec3f::dotProduct(direction, direction);
  const auto b = 2 * Vec3f::dotProduct(direction, L);
  const auto c = Vec3f::dotProduct(L, L) - radius2;
  auto solution = solveQuadratic(a, b, c);
  if (!solution.has_value()) {
    return false;
  }

  if (solution->x < 0) {
    solution->x = solution->y;
  }
  if (solution->x < 0) {
    return false;
  }

  tnear = solution->x;

  return true;
}

auto Sphere::surfaceProperties(const Vec3f &P, const Vec3f &, uint32_t,
                               const Vec2f &) const -> SurfaceProperties {
  return {Vec3f::normalize(P - center), Vec2f{0, 0}};
}
