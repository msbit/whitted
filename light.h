#ifndef __LIGHT__
#define __LIGHT__

#include "vec3f.h"

class Light {
public:
  Light(const Vec3f &, const Vec3f &);
  Vec3f position;
  Vec3f intensity;
};

#endif
