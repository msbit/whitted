#ifndef __LIGHT__
#define __LIGHT__

#include "vec3.h"

struct Light {
  Vec3<float> intensity;
  Vec3<float> position;

  Light(const Vec3<float>, const Vec3<float>);
};

#endif
