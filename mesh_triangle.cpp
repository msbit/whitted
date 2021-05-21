#include <cmath>
#include <cstring>
#include <optional>

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
                           const std::vector<uint32_t> vertexIndex,
                           uint32_t numTriangles,
                           const std::vector<Vec2<float>> st)
    : numTriangles(numTriangles), stCoordinates(st), vertexIndex(vertexIndex),
      vertices(vertices) {}

auto MeshTriangle::intersect(const Vec3<float> &origin,
                             const Vec3<float> &direction, float &tnear,
                             uint32_t &index, Vec2<float> &uv) const -> bool {
  auto intersect = false;
  for (auto k = 0; k < numTriangles; ++k) {
    const auto &v0 = vertices[vertexIndex[k * 3]];
    const auto &v1 = vertices[vertexIndex[k * 3 + 1]];
    const auto &v2 = vertices[vertexIndex[k * 3 + 2]];
    auto intersection = rayTriangleIntersect(v0, v1, v2, origin, direction);
    if (!intersection.has_value()) {
      continue;
    }
    if (intersection->x >= tnear) {
      continue;
    }

    tnear = intersection->x;
    uv.x = intersection->y;
    uv.y = intersection->z;
    index = k;
    intersect |= true;
  }

  return intersect;
}

auto MeshTriangle::surfaceProperties(const Vec3<float> &, const Vec3<float> &,
                                     uint32_t index,
                                     const Vec2<float> &uv) const
    -> SurfaceProperties {
  const auto v0 = vertices[vertexIndex[index * 3]];
  const auto v1 = vertices[vertexIndex[index * 3 + 1]];
  const auto v2 = vertices[vertexIndex[index * 3 + 2]];
  const auto e0 = Vec3<float>::normalize(v1 - v0);
  const auto e1 = Vec3<float>::normalize(v2 - v1);
  auto N = Vec3<float>::normalize(Vec3<float>::crossProduct(e0, e1));

  const auto st0 = stCoordinates[vertexIndex[index * 3]];
  const auto st1 = stCoordinates[vertexIndex[index * 3 + 1]];
  const auto st2 = stCoordinates[vertexIndex[index * 3 + 2]];
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
