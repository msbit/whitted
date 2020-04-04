#ifndef __OBJECT__
#define __OBJECT__

#include "vec2f.h"
#include "vec3f.h"

enum MaterialType { DIFFUSE_AND_GLOSSY, REFLECTION_AND_REFRACTION, REFLECTION };

class Object {
public:
  Object();
  virtual ~Object();
  virtual bool intersect(const Vec3f &, const Vec3f &, float &, uint32_t &,
                         Vec2f &) const = 0;
  virtual void getSurfaceProperties(const Vec3f &, const Vec3f &,
                                    const uint32_t &, const Vec2f &, Vec3f &,
                                    Vec2f &) const = 0;
  virtual Vec3f evalDiffuseColor(const Vec2f &) const;
  // material properties
  MaterialType materialType;
  float ior;
  float Kd;
  float Ks;
  Vec3f diffuseColor;
  float specularExponent;
};

#endif
