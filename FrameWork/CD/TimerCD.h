#pragma once
#include <Siv3D.hpp>

struct TimerCD {
  TimerCD() = default;
  TimerCD(double current_timer) : current_timer(current_timer) {}

  double current_timer = 0.0;  // 現在のタイマー値
};
