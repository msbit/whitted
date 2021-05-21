#include "light.h"

Light::Light(const Vec3<float> p, const Vec3<float> i)
    : intensity(i), position(p) {}
