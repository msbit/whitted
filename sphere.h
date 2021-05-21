#ifndef __SPHERE__
#define __SPHERE__

#include "object.h"
#include "vec2.h"
#include "vec3.h"

struct Sphere : public Object {
  Vec3<float> center;
  float radius;
  float radius2;

  Sphere(const Vec3<float>, float);

  auto intersect(const Vec3<float> &, const Vec3<float> &, float &, uint32_t &,
                 Vec2<float> &) const -> bool;
  auto surfaceProperties(const Vec3<float> &, const Vec3<float> &, uint32_t,
                         const Vec2<float> &) const -> SurfaceProperties;
};

#endif
