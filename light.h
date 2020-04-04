#ifndef __LIGHT__
#define __LIGHT__

#include "vec3f.h"

class Light {
public:
  Light(const Vec3f &p, const Vec3f &i) : position(p), intensity(i) {}
  Vec3f position;
  Vec3f intensity;
};

#endif
