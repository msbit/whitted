#ifndef __OBJECT__
#define __OBJECT__

#include "vec2f.h"
#include "vec3f.h"

enum MaterialType { DIFFUSE_AND_GLOSSY, REFLECTION_AND_REFRACTION, REFLECTION };

class Object {
public:
  Object()
      : materialType(DIFFUSE_AND_GLOSSY), ior(1.3), Kd(0.8), Ks(0.2),
        diffuseColor(0.2), specularExponent(25) {}
  virtual ~Object() {}
  virtual bool intersect(const Vec3f &, const Vec3f &, float &, uint32_t &,
                         Vec2f &) const = 0;
  virtual void getSurfaceProperties(const Vec3f &, const Vec3f &,
                                    const uint32_t &, const Vec2f &, Vec3f &,
                                    Vec2f &) const = 0;
  virtual Vec3f evalDiffuseColor(const Vec2f &) const { return diffuseColor; }
  // material properties
  MaterialType materialType;
  float ior;
  float Kd, Ks;
  Vec3f diffuseColor;
  float specularExponent;
};

#endif
