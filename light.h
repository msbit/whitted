#ifndef __LIGHT__
#define __LIGHT__

#include "vec3f.h"

struct Light {
  Light(const Vec3f &, const Vec3f &);
  Vec3f position;
  Vec3f intensity;
};

#endif
