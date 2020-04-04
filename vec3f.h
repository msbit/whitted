#ifndef __VEC3F__
#define __VEC3F__

#include <ostream>

class Vec3f {
public:
  Vec3f() : x(0), y(0), z(0) {}
  Vec3f(float xx) : x(xx), y(xx), z(xx) {}
  Vec3f(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}

  Vec3f operator*(const float &r) const { return Vec3f(x * r, y * r, z * r); }
  Vec3f operator*(const Vec3f &v) const {
    return Vec3f(x * v.x, y * v.y, z * v.z);
  }
  Vec3f operator-(const Vec3f &v) const {
    return Vec3f(x - v.x, y - v.y, z - v.z);
  }
  Vec3f operator+(const Vec3f &v) const {
    return Vec3f(x + v.x, y + v.y, z + v.z);
  }
  Vec3f operator-() const { return Vec3f(-x, -y, -z); }
  Vec3f &operator+=(const Vec3f &v) {
    x += v.x, y += v.y, z += v.z;
    return *this;
  }
  friend Vec3f operator*(const float &r, const Vec3f &v) {
    return Vec3f(v.x * r, v.y * r, v.z * r);
  }
  friend std::ostream &operator<<(std::ostream &os, const Vec3f &v) {
    return os << v.x << ", " << v.y << ", " << v.z;
  }

  static Vec3f crossProduct(const Vec3f &a, const Vec3f &b) {
    return Vec3f(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                 a.x * b.y - a.y * b.x);
  }

  static inline float dotProduct(const Vec3f &a, const Vec3f &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }

  static inline Vec3f mix(const Vec3f &a, const Vec3f &b, const float &mixValue) {
    return a * (1 - mixValue) + b * mixValue;
  }

  static Vec3f normalize(const Vec3f &v) {
    float mag2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (mag2 > 0) {
      float invMag = 1 / sqrtf(mag2);
      return Vec3f(v.x * invMag, v.y * invMag, v.z * invMag);
    }

    return v;
  }

  float x, y, z;
};

#endif
