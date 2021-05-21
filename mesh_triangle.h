#ifndef __MESH_TRIANGLE__
#define __MESH_TRIANGLE__

#include <vector>

#include "object.h"
#include "vec2.h"
#include "vec3.h"

struct MeshTriangle : public Object {
  std::vector<Vec2<float>> stCoordinates;
  std::vector<Vec3<uint32_t>> vertexIndex;
  std::vector<Vec3<float>> vertices;

  MeshTriangle(const std::vector<Vec3<float>>,
               const std::vector<Vec3<uint32_t>>,
               const std::vector<Vec2<float>>);

  auto intersect(const Vec3<float> &, const Vec3<float> &, float &, uint32_t &,
                 Vec2<float> &) const -> bool;
  auto surfaceProperties(const Vec3<float> &, const Vec3<float> &, uint32_t,
                         const Vec2<float> &) const -> SurfaceProperties;
  auto evalDiffuseColor(const Vec2<float> &) const -> Vec3<float>;
};

#endif
