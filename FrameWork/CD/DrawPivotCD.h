#pragma once
#include <Siv3D.hpp>

struct DrawPivotCD {
  DrawPivotCD() = default;
  DrawPivotCD(const Vec3& p) : pivot(p) {}
  Vec3 pivot = Vec3::Zero();
};
