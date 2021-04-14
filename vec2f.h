#ifndef __VEC2F__
#define __VEC2F__

struct Vec2f {
  float x;
  float y;

  Vec2f();
  Vec2f(float);
  Vec2f(float, float);

  Vec2f operator*(const float &) const;
  Vec2f operator+(const Vec2f &) const;
};

#endif
