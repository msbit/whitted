#include <cmath>

#include "vec3f.h"

Vec3f::Vec3f() : x(0), y(0), z(0) {}
Vec3f::Vec3f(float s) : x(s), y(s), z(s) {}
Vec3f::Vec3f(float x, float y, float z) : x(x), y(y), z(z) {}

auto Vec3f::operator*(float s) const -> Vec3f { return {x * s, y * s, z * s}; }
auto Vec3f::operator*(const Vec3f &other) const -> Vec3f {
  return {x * other.x, y * other.y, z * other.z};
}
auto Vec3f::operator-(const Vec3f &other) const -> Vec3f {
  return {x - other.x, y - other.y, z - other.z};
}
auto Vec3f::operator+(const Vec3f &other) const -> Vec3f {
  return {x + other.x, y + other.y, z + other.z};
}
auto Vec3f::operator-() const -> Vec3f { return {-x, -y, -z}; }
auto Vec3f::operator+=(const Vec3f &other) -> Vec3f & {
  x += other.x, y += other.y, z += other.z;
  return *this;
}

auto Vec3f::crossProduct(const Vec3f &a, const Vec3f &b) -> Vec3f {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

auto Vec3f::dotProduct(const Vec3f &a, const Vec3f &b) -> float {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

auto Vec3f::mix(const Vec3f &a, const Vec3f &b, float mixValue) -> Vec3f {
  return a * (1 - mixValue) + b * mixValue;
}

auto Vec3f::normalize(const Vec3f &v) -> Vec3f {
  const auto mag2 = v.x * v.x + v.y * v.y + v.z * v.z;
  if (mag2 == 0) {
    return v;
  }

  const auto invMag = 1 / std::sqrtf(mag2);
  return {v.x * invMag, v.y * invMag, v.z * invMag};
}

auto operator*(float s, const Vec3f &v) -> Vec3f {
  return {v.x * s, v.y * s, v.z * s};
}
