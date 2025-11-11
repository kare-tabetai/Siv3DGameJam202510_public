
#pragma once
#include <Siv3D.hpp>

#include "FrameWork/CD/HoveringTagCD.h"
#include "FrameWork/CD/SelectableCD.h"
#include "FrameWork/CD/SelectingTagCD.h"
#include "FrameWork/flecs/flecs.h"

class SelectSystem {
  public:
  static void Register(flecs::world& world) {
    world.system<SelectableCD>("select_sys")
      .each([&world](flecs::entity e, SelectableCD& selectable) {
        // マウス左クリックされたらSelectingTagCDを追加
        if (selectable.area.leftClicked()) {
          // 他のentityからSelectingTagCDを削除する
          flecs::query<> query =
            world.query_builder().with<SelectingTagCD>().build();

          query.each([e, &world](flecs::entity other) {
            if (other != e) {
              world.defer([other]() { other.remove<SelectingTagCD>(); });
            }
          });

          // 自分にSelectingTagCDを追加
          if (!e.has<SelectingTagCD>()) {
            world.defer([e]() { e.add<SelectingTagCD>(); });
          }
        }
      });
  }
};