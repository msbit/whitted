#ifndef __VEC3F__
#define __VEC3F__

#include <ostream>

struct Vec3f {
  Vec3f();
  Vec3f(float);
  Vec3f(float, float, float);

  Vec3f operator*(const float &) const;
  Vec3f operator*(const Vec3f &) const;
  Vec3f operator-(const Vec3f &) const;
  Vec3f operator+(const Vec3f &) const;
  Vec3f operator-() const;
  Vec3f &operator+=(const Vec3f &);

  friend Vec3f operator*(const float &, const Vec3f &);
  friend std::ostream &operator<<(std::ostream &, const Vec3f &);

  static Vec3f crossProduct(const Vec3f &, const Vec3f &);

  static float dotProduct(const Vec3f &, const Vec3f &);

  static Vec3f mix(const Vec3f &, const Vec3f &, const float &);

  static Vec3f normalize(const Vec3f &);

  float x;
  float y;
  float z;
};

#endif
