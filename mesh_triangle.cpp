#include <cmath>
#include <cstring>
#include <optional>
#include <tuple>

#include "mesh_triangle.h"

auto rayTriangleIntersect(const Vec3<float> &v0, const Vec3<float> &v1,
                          const Vec3<float> &v2, const Vec3<float> &origin,
                          const Vec3<float> &direction)
    -> std::optional<Vec3<float>> {
  const auto edge1 = v1 - v0;
  const auto edge2 = v2 - v0;
  const auto pvec = Vec3<float>::crossProduct(direction, edge2);
  const auto det = Vec3<float>::dotProduct(edge1, pvec);
  if (det == 0 || det < 0) {
    return std::nullopt;
  }

  const auto tvec = origin - v0;
  auto u = Vec3<float>::dotProduct(tvec, pvec);
  if (u < 0 || u > det) {
    return std::nullopt;
  }

  const auto qvec = Vec3<float>::crossProduct(tvec, edge1);
  auto v = Vec3<float>::dotProduct(direction, qvec);
  if (v < 0 || u + v > det) {
    return std::nullopt;
  }

  const auto invDet = 1 / det;

  auto tnear = Vec3<float>::dotProduct(edge2, qvec) * invDet;
  u *= invDet;
  v *= invDet;

  return Vec3<float>{tnear, u, v};
}

MeshTriangle::MeshTriangle(const std::vector<Vec3<float>> vertices,
                           const std::vector<Vec3<uint32_t>> vertexIndex,
                           const std::vector<Vec2<float>> st)
    : stCoordinates(st), vertexIndex(vertexIndex), vertices(vertices) {}

auto MeshTriangle::intersect(const Vec3<float> &origin,
                             const Vec3<float> &direction) const
    -> std::optional<std::tuple<float, uint32_t, Vec2<float>>> {
  auto hits = std::vector<std::optional<Vec3<float>>>();
  std::transform(vertexIndex.begin(), vertexIndex.end(),
                 std::back_inserter(hits), [this, direction, origin](auto &v) {
                   const auto &v0 = vertices[v.x];
                   const auto &v1 = vertices[v.y];
                   const auto &v2 = vertices[v.z];
                   return rayTriangleIntersect(v0, v1, v2, origin, direction);
                 });

  if (!std::any_of(hits.begin(), hits.end(),
                   [](auto i) { return i.has_value(); })) {
    return std::nullopt;
  }

  auto it = std::min_element(hits.begin(), hits.end(),
                             [](const auto &a, const auto &b) {
                               if (!b.has_value()) {
                                 return a.has_value();
                               }
                               if (!a.has_value() && b.has_value()) {
                                 return false;
                               }
                               return a->x < b->x;
                             });

  auto found = *it;
  return std::make_tuple(found->x, it - hits.begin(),
                         Vec2<float>(found->y, found->z));
}

auto MeshTriangle::surfaceProperties(const Vec3<float> &, const Vec3<float> &,
                                     uint32_t index,
                                     const Vec2<float> &uv) const
    -> SurfaceProperties {
  const auto v = vertexIndex[index];
  const auto v0 = vertices[v.x];
  const auto v1 = vertices[v.y];
  const auto v2 = vertices[v.z];
  const auto e0 = Vec3<float>::normalize(v1 - v0);
  const auto e1 = Vec3<float>::normalize(v2 - v1);
  auto N = Vec3<float>::normalize(Vec3<float>::crossProduct(e0, e1));

  const auto st0 = stCoordinates[v.x];
  const auto st1 = stCoordinates[v.y];
  const auto st2 = stCoordinates[v.z];
  auto st = st0 * (1 - uv.x - uv.y) + st1 * uv.x + st2 * uv.y;

  return {N, st};
}

auto MeshTriangle::evalDiffuseColor(const Vec2<float> &st) const
    -> Vec3<float> {
  const auto scale = 5.f;
  const auto pattern =
      (std::fmodf(st.x * scale, 1) > 0.5) ^ (std::fmodf(st.y * scale, 1) > 0.5);
  return Vec3<float>::mix({0.815, 0.235, 0.031}, {0.937, 0.937, 0.231},
                          pattern);
}
