#ifndef __LIGHT__
#define __LIGHT__

#include "vec3f.h"

struct Light {
  Vec3f position;
  Vec3f intensity;

  Light(const Vec3f, const Vec3f);
};

#endif
