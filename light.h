#ifndef __LIGHT__
#define __LIGHT__

#include "vec3f.h"

struct Light {
  Vec3f intensity;
  Vec3f position;

  Light(const Vec3f, const Vec3f);
};

#endif
