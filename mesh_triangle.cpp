#include <cmath>
#include <cstring>
#include <optional>

#include "mesh_triangle.h"

std::optional<Vec3f> rayTriangleIntersect(const Vec3f &v0, const Vec3f &v1,
                                          const Vec3f &v2, const Vec3f &origin,
                                          const Vec3f &direction) {
  const auto edge1 = v1 - v0;
  const auto edge2 = v2 - v0;
  const auto pvec = Vec3f::crossProduct(direction, edge2);
  const auto det = Vec3f::dotProduct(edge1, pvec);
  if (det == 0 || det < 0) {
    return std::nullopt;
  }

  const auto tvec = origin - v0;
  auto u = Vec3f::dotProduct(tvec, pvec);
  if (u < 0 || u > det) {
    return std::nullopt;
  }

  const auto qvec = Vec3f::crossProduct(tvec, edge1);
  auto v = Vec3f::dotProduct(direction, qvec);
  if (v < 0 || u + v > det) {
    return std::nullopt;
  }

  const auto invDet = 1 / det;

  auto tnear = Vec3f::dotProduct(edge2, qvec) * invDet;
  u *= invDet;
  v *= invDet;

  return Vec3f{tnear, u, v};
}

MeshTriangle::MeshTriangle(const std::vector<Vec3f> vertices,
                           const std::vector<uint32_t> vertexIndex,
                           uint32_t numTriangles, const std::vector<Vec2f> st)
    : numTriangles(numTriangles), stCoordinates(st), vertexIndex(vertexIndex),
      vertices(vertices) {}

bool MeshTriangle::intersect(const Vec3f &origin, const Vec3f &direction,
                             float &tnear, uint32_t &index, Vec2f &uv) const {
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

SurfaceProperties MeshTriangle::surfaceProperties(const Vec3f &, const Vec3f &,
                                                  uint32_t index,
                                                  const Vec2f &uv) const {
  const auto v0 = vertices[vertexIndex[index * 3]];
  const auto v1 = vertices[vertexIndex[index * 3 + 1]];
  const auto v2 = vertices[vertexIndex[index * 3 + 2]];
  const auto e0 = Vec3f::normalize(v1 - v0);
  const auto e1 = Vec3f::normalize(v2 - v1);
  auto N = Vec3f::normalize(Vec3f::crossProduct(e0, e1));

  const auto st0 = stCoordinates[vertexIndex[index * 3]];
  const auto st1 = stCoordinates[vertexIndex[index * 3 + 1]];
  const auto st2 = stCoordinates[vertexIndex[index * 3 + 2]];
  auto st = st0 * (1 - uv.x - uv.y) + st1 * uv.x + st2 * uv.y;

  return {N, st};
}

Vec3f MeshTriangle::evalDiffuseColor(const Vec2f &st) const {
  const auto scale = 5.f;
  const auto pattern =
      (std::fmodf(st.x * scale, 1) > 0.5) ^ (std::fmodf(st.y * scale, 1) > 0.5);
  return Vec3f::mix({0.815, 0.235, 0.031}, {0.937, 0.937, 0.231}, pattern);
}
