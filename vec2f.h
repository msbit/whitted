#ifndef __VEC2F__
#define __VEC2F__

class Vec2f {
public:
  Vec2f() : x(0), y(0) {}
  Vec2f(float xx) : x(xx), y(xx) {}
  Vec2f(float xx, float yy) : x(xx), y(yy) {}
  Vec2f operator*(const float &r) const { return Vec2f(x * r, y * r); }
  Vec2f operator+(const Vec2f &v) const { return Vec2f(x + v.x, y + v.y); }
  float x, y;
};

#endif
