#include <cmath>
#include <optional>
#include <utility>

#include "sphere.h"

auto solveQuadratic(float a, float b, float c) -> std::optional<Vec2<float>> {
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
  return Vec2<float>{x0, x1};
}

Sphere::Sphere(const Vec3<float> c, float r)
    : center(c), radius(r), radius2(r * r) {}

auto Sphere::intersect(const Vec3<float> &origin,
                       const Vec3<float> &direction) const
    -> std::optional<std::tuple<float, uint32_t, Vec2<float>>> {
  // analytic solution
  const auto L = origin - center;
  const auto a = Vec3<float>::dotProduct(direction, direction);
  const auto b = 2 * Vec3<float>::dotProduct(direction, L);
  const auto c = Vec3<float>::dotProduct(L, L) - radius2;
  auto solution = solveQuadratic(a, b, c);
  if (!solution.has_value()) {
    return std::nullopt;
  }

  if (solution->x < 0) {
    solution->x = solution->y;
  }
  if (solution->x < 0) {
    return std::nullopt;
  }

  return std::make_tuple(solution->x, 0, Vec2<float>(0));
}

auto Sphere::surfaceProperties(const Vec3<float> &P, const Vec3<float> &,
                               uint32_t, const Vec2<float> &) const
    -> SurfaceProperties {
  return {Vec3<float>::normalize(P - center), Vec2<float>{0, 0}};
}
