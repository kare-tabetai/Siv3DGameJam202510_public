#pragma once
#include <Siv3D.hpp>

struct FrameCD {
  FrameCD() = default;
  FrameCD(int frame) : frame(frame) {}

  int frame = 0;  // 現在のフレーム番号
};
