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
  float Kd;
  float Ks;
  Vec3f diffuseColor;
  float ior;
  MaterialType materialType;
  float specularExponent;

  Object();
  virtual ~Object();

  virtual auto intersect(const Vec3f &, const Vec3f &, float &, uint32_t &,
                         Vec2f &) const -> bool = 0;
  virtual auto surfaceProperties(const Vec3f &, const Vec3f &, uint32_t,
                                 const Vec2f &) const -> SurfaceProperties = 0;
  virtual auto evalDiffuseColor(const Vec2f &) const -> Vec3f;
};

#endif
