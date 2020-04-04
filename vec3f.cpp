#include <cmath>
#include <ostream>

#include "vec3f.h"

Vec3f::Vec3f() : x(0), y(0), z(0) {}
Vec3f::Vec3f(float s) : x(s), y(s), z(s) {}
Vec3f::Vec3f(float x, float y, float z) : x(x), y(y), z(z) {}

Vec3f Vec3f::operator*(const float &r) const { return {x * r, y * r, z * r}; }
Vec3f Vec3f::operator*(const Vec3f &v) const {
  return {x * v.x, y * v.y, z * v.z};
}
Vec3f Vec3f::operator-(const Vec3f &v) const {
  return {x - v.x, y - v.y, z - v.z};
}
Vec3f Vec3f::operator+(const Vec3f &v) const {
  return {x + v.x, y + v.y, z + v.z};
}
Vec3f Vec3f::operator-() const { return {-x, -y, -z}; }
Vec3f &Vec3f::operator+=(const Vec3f &v) {
  x += v.x, y += v.y, z += v.z;
  return *this;
}

Vec3f Vec3f::crossProduct(const Vec3f &a, const Vec3f &b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

float Vec3f::dotProduct(const Vec3f &a, const Vec3f &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3f Vec3f::mix(const Vec3f &a, const Vec3f &b, const float &mixValue) {
  return a * (1 - mixValue) + b * mixValue;
}

Vec3f Vec3f::normalize(const Vec3f &v) {
  float mag2 = v.x * v.x + v.y * v.y + v.z * v.z;
  if (mag2 > 0) {
    float invMag = 1 / sqrtf(mag2);
    return {v.x * invMag, v.y * invMag, v.z * invMag};
  }

  return v;
}

Vec3f operator*(const float &r, const Vec3f &v) {
  return {v.x * r, v.y * r, v.z * r};
}

std::ostream &operator<<(std::ostream &os, const Vec3f &v) {
  return os << v.x << ", " << v.y << ", " << v.z;
}
