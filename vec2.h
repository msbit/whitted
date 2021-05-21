#ifndef __VEC2F__
#define __VEC2F__

template <class T> struct Vec2 {
  T x;
  T y;

  Vec2() : x(0), y(0) {}
  Vec2(T s) : x(s), y(s) {}
  Vec2(T x, T y) : x(x), y(y) {}

  auto operator*(T s) const -> Vec2 { return {x * s, y * s}; }
  auto operator+(const Vec2 &other) const -> Vec2 {
    return {x + other.x, y + other.y};
  }
};

#endif
