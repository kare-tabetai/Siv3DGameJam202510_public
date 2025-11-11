#pragma once
#include <Siv3D.hpp>

#include "CD/TimerCD.h"
#include "flecs/flecs.h"

class TimerSystem {
  public:
  static void Register(flecs::world& world) {
    auto timer_sys = world.system<TimerCD>("timer_sys")
                       .kind(flecs::PreUpdate)
                       .without(flecs::Prefab)
                       .each([](TimerCD& timer) {
                         // デルタタイムを加算してタイマーを更新
                         timer.current_timer += Scene::DeltaTime();
                       });
  }
};
