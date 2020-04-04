#include "object.h"

Object::Object()
    : materialType(DIFFUSE_AND_GLOSSY), ior(1.3), Kd(0.8), Ks(0.2),
      diffuseColor(0.2), specularExponent(25) {}
Object::~Object() {}
Vec3f Object::evalDiffuseColor(const Vec2f &) const { return diffuseColor; }
