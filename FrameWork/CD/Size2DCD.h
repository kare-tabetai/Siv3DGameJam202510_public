#pragma once
#include <Siv3D.hpp>

struct Size2DCD {
  Size2DCD() = default;
  Size2DCD(double w, double h) : size(w, h) {}
  Size2DCD(const Vec2& s) : size(s) {}

  Vec2 size = Vec2(1.0, 1.0);
};
