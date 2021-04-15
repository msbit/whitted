#ifndef __OBJECT__
#define __OBJECT__

#include <cstdint>

#include "vec2f.h"
#include "vec3f.h"

enum MaterialType { DIFFUSE_AND_GLOSSY, REFLECTION_AND_REFRACTION, REFLECTION };

struct SurfaceProperties {
  Vec3f N;
  Vec2f st;
};

struct Object {
  // material properties
  MaterialType materialType;
  float ior;
  float Kd;
  float Ks;
  Vec3f diffuseColor;
  float specularExponent;

  Object();
  virtual ~Object();

  virtual bool intersect(const Vec3f &, const Vec3f &, float &, uint32_t &,
                         Vec2f &) const = 0;
  virtual SurfaceProperties surfaceProperties(const Vec3f &, const Vec3f &,
                                              uint32_t,
                                              const Vec2f &) const = 0;
  virtual Vec3f evalDiffuseColor(const Vec2f &) const;
};

#endif
