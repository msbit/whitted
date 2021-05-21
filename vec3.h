#ifndef __VEC3F__
#define __VEC3F__

#include <cmath>

template <class T> struct Vec3 {
  T x;
  T y;
  T z;

  Vec3() : x(0), y(0), z(0) {}
  Vec3(T s) : x(s), y(s), z(s) {}
  Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

  auto operator*(T s) const -> Vec3 { return {x * s, y * s, z * s}; }
  auto operator*(const Vec3 &other) const -> Vec3 {
    return {x * other.x, y * other.y, z * other.z};
  }
  auto operator-(const Vec3 &other) const -> Vec3 {
    return {x - other.x, y - other.y, z - other.z};
  }
  auto operator+(const Vec3 &other) const -> Vec3 {
    return {x + other.x, y + other.y, z + other.z};
  }
  auto operator-() const -> Vec3 { return {-x, -y, -z}; }
  auto operator+=(const Vec3 &other) -> Vec3 & {
    x += other.x, y += other.y, z += other.z;
    return *this;
  }

  friend auto operator*(T s, const Vec3 &v) -> Vec3 {
    return {v.x * s, v.y * s, v.z * s};
  }

  static auto crossProduct(const Vec3 &a, const Vec3 &b) -> Vec3 {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
  }
  static auto dotProduct(const Vec3 &a, const Vec3 &b) -> T {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }
  static auto mix(const Vec3 &a, const Vec3 &b, T mixValue) -> Vec3 {
    return a * (1 - mixValue) + b * mixValue;
  }
  static auto normalize(const Vec3 &v) -> Vec3 {
    const auto mag2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (mag2 == 0) {
      return v;
    }

    const auto invMag = 1 / std::sqrtf(mag2);
    return {v.x * invMag, v.y * invMag, v.z * invMag};
  }
};

#endif
