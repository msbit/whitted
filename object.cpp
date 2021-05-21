#include "object.h"

Object::Object()
    : Kd(0.8), Ks(0.2), diffuseColor(0.2), ior(1.3),
      materialType(DIFFUSE_AND_GLOSSY), specularExponent(25) {}
Object::~Object() {}
auto Object::evalDiffuseColor(const Vec2<float> &) const -> Vec3<float> {
  return diffuseColor;
}
