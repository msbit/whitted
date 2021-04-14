#ifndef __SPHERE__
#define __SPHERE__

#include "object.h"
#include "vec2f.h"
#include "vec3f.h"

struct Sphere : public Object {
  Sphere(const Vec3f &, const float &);

  bool intersect(const Vec3f &, const Vec3f &, float &, uint32_t &,
                 Vec2f &) const;

  void getSurfaceProperties(const Vec3f &, const Vec3f &, const uint32_t &,
                            const Vec2f &, Vec3f &, Vec2f &) const;

  Vec3f center;
  float radius;
  float radius2;
};

#endif
