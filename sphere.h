#ifndef __SPHERE__
#define __SPHERE__

#include "object.h"
#include "vec2f.h"
#include "vec3f.h"

struct Sphere : public Object {
  Vec3f center;
  float radius;
  float radius2;

  Sphere(const Vec3f, float);

  auto intersect(const Vec3f &, const Vec3f &, float &, uint32_t &,
                 Vec2f &) const -> bool;
  auto surfaceProperties(const Vec3f &, const Vec3f &, uint32_t,
                         const Vec2f &) const -> SurfaceProperties;
};

#endif
