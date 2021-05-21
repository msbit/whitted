#ifndef __VEC3F__
#define __VEC3F__

struct Vec3f {
  float x;
  float y;
  float z;

  Vec3f();
  Vec3f(float);
  Vec3f(float, float, float);

  auto operator*(float) const -> Vec3f;
  auto operator*(const Vec3f &) const -> Vec3f;
  auto operator-(const Vec3f &) const -> Vec3f;
  auto operator+(const Vec3f &) const -> Vec3f;
  auto operator-() const -> Vec3f;
  auto operator+=(const Vec3f &) -> Vec3f &;

  friend auto operator*(float, const Vec3f &) -> Vec3f;

  static auto crossProduct(const Vec3f &, const Vec3f &) -> Vec3f;
  static auto dotProduct(const Vec3f &, const Vec3f &) -> float;
  static auto mix(const Vec3f &, const Vec3f &, float) -> Vec3f;
  static auto normalize(const Vec3f &) -> Vec3f;
};

#endif
