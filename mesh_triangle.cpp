#include <cmath>
#include <cstring>
#include <optional>

#include "mesh_triangle.h"

std::optional<Vec3f> rayTriangleIntersect(const Vec3f &v0, const Vec3f &v1,
                                          const Vec3f &v2, const Vec3f &origin,
                                          const Vec3f &direction) {
  const Vec3f edge1 = v1 - v0;
  const Vec3f edge2 = v2 - v0;
  const Vec3f pvec = Vec3f::crossProduct(direction, edge2);
  const float det = Vec3f::dotProduct(edge1, pvec);
  if (det == 0 || det < 0) {
    return std::nullopt;
  }

  const Vec3f tvec = origin - v0;
  float u = Vec3f::dotProduct(tvec, pvec);
  if (u < 0 || u > det) {
    return std::nullopt;
  }

  const Vec3f qvec = Vec3f::crossProduct(tvec, edge1);
  float v = Vec3f::dotProduct(direction, qvec);
  if (v < 0 || u + v > det) {
    return std::nullopt;
  }

  const float invDet = 1 / det;

  float tnear = Vec3f::dotProduct(edge2, qvec) * invDet;
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
  bool intersect = false;
  for (uint32_t k = 0; k < numTriangles; ++k) {
    const Vec3f &v0 = vertices[vertexIndex[k * 3]];
    const Vec3f &v1 = vertices[vertexIndex[k * 3 + 1]];
    const Vec3f &v2 = vertices[vertexIndex[k * 3 + 2]];
    std::optional<Vec3f> intersection =
        rayTriangleIntersect(v0, v1, v2, origin, direction);
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
  const Vec3f v0 = vertices[vertexIndex[index * 3]];
  const Vec3f v1 = vertices[vertexIndex[index * 3 + 1]];
  const Vec3f v2 = vertices[vertexIndex[index * 3 + 2]];
  const Vec3f e0 = Vec3f::normalize(v1 - v0);
  const Vec3f e1 = Vec3f::normalize(v2 - v1);
  auto N = Vec3f::normalize(Vec3f::crossProduct(e0, e1));

  const Vec2f st0 = stCoordinates[vertexIndex[index * 3]];
  const Vec2f st1 = stCoordinates[vertexIndex[index * 3 + 1]];
  const Vec2f st2 = stCoordinates[vertexIndex[index * 3 + 2]];
  auto st = st0 * (1 - uv.x - uv.y) + st1 * uv.x + st2 * uv.y;

  return {N, st};
}

Vec3f MeshTriangle::evalDiffuseColor(const Vec2f &st) const {
  const float scale = 5;
  const float pattern =
      (fmodf(st.x * scale, 1) > 0.5) ^ (fmodf(st.y * scale, 1) > 0.5);
  return Vec3f::mix({0.815, 0.235, 0.031}, {0.937, 0.937, 0.231}, pattern);
}
