#ifndef __MESH_TRIANGLE__
#define __MESH_TRIANGLE__

#include <vector>

#include "object.h"
#include "vec2f.h"
#include "vec3f.h"

struct MeshTriangle : public Object {
  MeshTriangle(const std::vector<Vec3f> &, const std::vector<uint32_t> &,
               const uint32_t &, const std::vector<Vec2f> &);

  bool intersect(const Vec3f &, const Vec3f &, float &, uint32_t &,
                 Vec2f &) const;

  void getSurfaceProperties(const Vec3f &, const Vec3f &, const uint32_t &,
                            const Vec2f &, Vec3f &, Vec2f &) const;

  Vec3f evalDiffuseColor(const Vec2f &) const;

  std::vector<Vec3f> vertices;
  uint32_t numTriangles;
  std::vector<uint32_t> vertexIndex;
  std::vector<Vec2f> stCoordinates;
};

#endif
