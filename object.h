#ifndef __OBJECT__
#define __OBJECT__

#include <cstdint>

#include "vec2.h"
#include "vec3.h"

enum MaterialType { DIFFUSE_AND_GLOSSY, REFLECTION_AND_REFRACTION, REFLECTION };

struct SurfaceProperties {
  Vec3<float> N;
  Vec2<float> st;
};

struct Object {
  // material properties
  float Kd;
  float Ks;
  Vec3<float> diffuseColor;
  float ior;
  MaterialType materialType;
  float specularExponent;

  Object();
  virtual ~Object();

  virtual auto intersect(const Vec3<float> &, const Vec3<float> &, float &,
                         uint32_t &, Vec2<float> &) const -> bool = 0;
  virtual auto surfaceProperties(const Vec3<float> &, const Vec3<float> &,
                                 uint32_t, const Vec2<float> &) const
      -> SurfaceProperties = 0;
  virtual auto evalDiffuseColor(const Vec2<float> &) const -> Vec3<float>;
};

#endif
