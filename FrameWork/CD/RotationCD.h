#pragma once
#include <Siv3D.hpp>

struct RotationCD {
  RotationCD() = default;
  RotationCD(const Quaternion& p) : rot(p) {}

  Quaternion rot = Quaternion::Identity();
};
