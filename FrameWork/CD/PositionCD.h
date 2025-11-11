#pragma once
#include <Siv3D.hpp>

struct PositionCD {
  PositionCD() = default;
  PositionCD(const Vec3& p) : pos(p) {}
  Vec3 pos = Vec3::Zero();
};
