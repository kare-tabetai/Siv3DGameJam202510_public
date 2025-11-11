#pragma once
#include <Siv3D.hpp>

struct SpriteAnimationCD {
  SpriteAnimationCD() = default;
  SpriteAnimationCD(double interval, bool loop = true)
      : interval(interval), loop(loop) {}

  double interval = 10;  // フレームが切り替わるまでの間隔
  bool loop = true;      // ループ再生するかどうか
};
