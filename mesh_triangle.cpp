#include <cstring>

#include <cmath>

#include "mesh_triangle.h"

bool rayTriangleIntersect(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2,
                          const Vec3f &origin, const Vec3f &direction,
                          float &tnear, float &u, float &v) {
  const Vec3f edge1 = v1 - v0;
  const Vec3f edge2 = v2 - v0;
  const Vec3f pvec = Vec3f::crossProduct(direction, edge2);
  const float det = Vec3f::dotProduct(edge1, pvec);
  if (det == 0 || det < 0) {
    return false;
  }

  const Vec3f tvec = origin - v0;
  u = Vec3f::dotProduct(tvec, pvec);
  if (u < 0 || u > det) {
    return false;
  }

  const Vec3f qvec = Vec3f::crossProduct(tvec, edge1);
  v = Vec3f::dotProduct(direction, qvec);
  if (v < 0 || u + v > det) {
    return false;
  }

  const float invDet = 1 / det;

  tnear = Vec3f::dotProduct(edge2, qvec) * invDet;
  u *= invDet;
  v *= invDet;

  return true;
}

MeshTriangle::MeshTriangle(const std::vector<Vec3f> verts,
                           const std::vector<uint32_t> vertsIndex,
                           uint32_t numTriangles, const std::vector<Vec2f> st)
    : numTriangles(numTriangles), vertices(verts), stCoordinates(st),
      vertexIndex(vertsIndex) {}

bool MeshTriangle::intersect(const Vec3f &origin, const Vec3f &direction,
                             float &tnear, uint32_t &index, Vec2f &uv) const {
  bool intersect = false;
  for (uint32_t k = 0; k < numTriangles; ++k) {
    const Vec3f &v0 = vertices[vertexIndex[k * 3]];
    const Vec3f &v1 = vertices[vertexIndex[k * 3 + 1]];
    const Vec3f &v2 = vertices[vertexIndex[k * 3 + 2]];
    float t;
    float u;
    float v;
    if (rayTriangleIntersect(v0, v1, v2, origin, direction, t, u, v) &&
        t < tnear) {
      tnear = t;
      uv.x = u;
      uv.y = v;
      index = k;
      intersect |= true;
    }
  }

  return intersect;
}

void MeshTriangle::getSurfaceProperties(const Vec3f &P, const Vec3f &I,
                                        uint32_t index, const Vec2f &uv,
                                        Vec3f &N, Vec2f &st) const {
  const Vec3f &v0 = vertices[vertexIndex[index * 3]];
  const Vec3f &v1 = vertices[vertexIndex[index * 3 + 1]];
  const Vec3f &v2 = vertices[vertexIndex[index * 3 + 2]];
  const Vec3f e0 = Vec3f::normalize(v1 - v0);
  const Vec3f e1 = Vec3f::normalize(v2 - v1);
  N = Vec3f::normalize(Vec3f::crossProduct(e0, e1));
  const Vec2f &st0 = stCoordinates[vertexIndex[index * 3]];
  const Vec2f &st1 = stCoordinates[vertexIndex[index * 3 + 1]];
  const Vec2f &st2 = stCoordinates[vertexIndex[index * 3 + 2]];
  st = st0 * (1 - uv.x - uv.y) + st1 * uv.x + st2 * uv.y;
}

Vec3f MeshTriangle::evalDiffuseColor(const Vec2f &st) const {
  const float scale = 5;
  const float pattern =
      (fmodf(st.x * scale, 1) > 0.5) ^ (fmodf(st.y * scale, 1) > 0.5);
  return Vec3f::mix({0.815, 0.235, 0.031}, {0.937, 0.937, 0.231}, pattern);
}
