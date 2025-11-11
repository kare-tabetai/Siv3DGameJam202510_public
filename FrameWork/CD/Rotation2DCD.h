#pragma once
#include <Siv3D.hpp>

struct Rotation2DCD {
  Rotation2DCD() = default;
  Rotation2DCD(double angle) : rotation(angle) {}

  double rotation = 0.0;  // ラジアン単位での回転角度
};
