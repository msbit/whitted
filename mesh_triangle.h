#ifndef __MESH_TRIANGLE__
#define __MESH_TRIANGLE__

#include <memory>

#include "object.h"
#include "vec2f.h"
#include "vec3f.h"

class MeshTriangle : public Object {
public:
  MeshTriangle(const Vec3f *, const uint32_t *, const uint32_t &,
               const Vec2f *);

  bool intersect(const Vec3f &, const Vec3f &, float &, uint32_t &,
                 Vec2f &) const;

  void getSurfaceProperties(const Vec3f &, const Vec3f &, const uint32_t &,
                            const Vec2f &, Vec3f &, Vec2f &) const;

  Vec3f evalDiffuseColor(const Vec2f &) const;

  std::unique_ptr<Vec3f[]> vertices;
  uint32_t numTriangles;
  std::unique_ptr<uint32_t[]> vertexIndex;
  std::unique_ptr<Vec2f[]> stCoordinates;
};

#endif
