#include "vec2f.h"

Vec2f::Vec2f() : x(0), y(0) {}
Vec2f::Vec2f(float s) : x(s), y(s) {}
Vec2f::Vec2f(float x, float y) : x(x), y(y) {}
auto Vec2f::operator*(float s) const -> Vec2f { return {x * s, y * s}; }
auto Vec2f::operator+(const Vec2f &other) const -> Vec2f {
  return {x + other.x, y + other.y};
}
