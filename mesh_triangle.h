#ifndef __MESH_TRIANGLE__
#define __MESH_TRIANGLE__

#include <vector>

#include "object.h"
#include "vec2f.h"
#include "vec3f.h"

struct MeshTriangle : public Object {
  uint32_t numTriangles;
  std::vector<Vec2f> stCoordinates;
  std::vector<uint32_t> vertexIndex;
  std::vector<Vec3f> vertices;

  MeshTriangle(const std::vector<Vec3f>, const std::vector<uint32_t>, uint32_t,
               const std::vector<Vec2f>);

  auto intersect(const Vec3f &, const Vec3f &, float &, uint32_t &,
                 Vec2f &) const -> bool;
  auto surfaceProperties(const Vec3f &, const Vec3f &, uint32_t,
                         const Vec2f &) const -> SurfaceProperties;
  auto evalDiffuseColor(const Vec2f &) const -> Vec3f;
};

#endif
