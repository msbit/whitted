#include <cstring>

#include <cmath>

#include "mesh_triangle.h"

bool rayTriangleIntersect(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2,
                          const Vec3f &orig, const Vec3f &dir, float &tnear,
                          float &u, float &v) {
  Vec3f edge1 = v1 - v0;
  Vec3f edge2 = v2 - v0;
  Vec3f pvec = Vec3f::crossProduct(dir, edge2);
  float det = Vec3f::dotProduct(edge1, pvec);
  if (det == 0 || det < 0) {
    return false;
  }

  Vec3f tvec = orig - v0;
  u = Vec3f::dotProduct(tvec, pvec);
  if (u < 0 || u > det) {
    return false;
  }

  Vec3f qvec = Vec3f::crossProduct(tvec, edge1);
  v = Vec3f::dotProduct(dir, qvec);
  if (v < 0 || u + v > det) {
    return false;
  }

  float invDet = 1 / det;

  tnear = Vec3f::dotProduct(edge2, qvec) * invDet;
  u *= invDet;
  v *= invDet;

  return true;
}

MeshTriangle::MeshTriangle(const Vec3f *verts, const uint32_t *vertsIndex,
                           const uint32_t &numTris, const Vec2f *st) {
  uint32_t maxIndex = 0;
  for (uint32_t i = 0; i < numTris * 3; ++i) {
    if (vertsIndex[i] > maxIndex) {
      maxIndex = vertsIndex[i];
    }
  }
  maxIndex += 1;
  vertices = std::unique_ptr<Vec3f[]>(new Vec3f[maxIndex]);
  memcpy(vertices.get(), verts, sizeof(Vec3f) * maxIndex);
  vertexIndex = std::unique_ptr<uint32_t[]>(new uint32_t[numTris * 3]);
  memcpy(vertexIndex.get(), vertsIndex, sizeof(uint32_t) * numTris * 3);
  numTriangles = numTris;
  stCoordinates = std::unique_ptr<Vec2f[]>(new Vec2f[maxIndex]);
  memcpy(stCoordinates.get(), st, sizeof(Vec2f) * maxIndex);
}

bool MeshTriangle::intersect(const Vec3f &orig, const Vec3f &dir, float &tnear,
                             uint32_t &index, Vec2f &uv) const {
  bool intersect = false;
  for (uint32_t k = 0; k < numTriangles; ++k) {
    const Vec3f &v0 = vertices[vertexIndex[k * 3]];
    const Vec3f &v1 = vertices[vertexIndex[k * 3 + 1]];
    const Vec3f &v2 = vertices[vertexIndex[k * 3 + 2]];
    float t, u, v;
    if (rayTriangleIntersect(v0, v1, v2, orig, dir, t, u, v) && t < tnear) {
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
                                        const uint32_t &index, const Vec2f &uv,
                                        Vec3f &N, Vec2f &st) const {
  const Vec3f &v0 = vertices[vertexIndex[index * 3]];
  const Vec3f &v1 = vertices[vertexIndex[index * 3 + 1]];
  const Vec3f &v2 = vertices[vertexIndex[index * 3 + 2]];
  Vec3f e0 = Vec3f::normalize(v1 - v0);
  Vec3f e1 = Vec3f::normalize(v2 - v1);
  N = Vec3f::normalize(Vec3f::crossProduct(e0, e1));
  const Vec2f &st0 = stCoordinates[vertexIndex[index * 3]];
  const Vec2f &st1 = stCoordinates[vertexIndex[index * 3 + 1]];
  const Vec2f &st2 = stCoordinates[vertexIndex[index * 3 + 2]];
  st = st0 * (1 - uv.x - uv.y) + st1 * uv.x + st2 * uv.y;
}

Vec3f MeshTriangle::evalDiffuseColor(const Vec2f &st) const {
  float scale = 5;
  float pattern =
      (fmodf(st.x * scale, 1) > 0.5) ^ (fmodf(st.y * scale, 1) > 0.5);
  return Vec3f::mix(Vec3f(0.815, 0.235, 0.031), Vec3f(0.937, 0.937, 0.231),
                    pattern);
}
