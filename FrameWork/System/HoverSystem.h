

#pragma once
#include "FrameWork/CD/HoveringTagCD.h"
#include "FrameWork/CD/SelectableCD.h"
#include "FrameWork/flecs/flecs.h"

class HoverSystem {
  public:
  static void Register(flecs::world& world) {
    // SelectableCDを持つentityをイテレートして、
    // SelectableCD::areaにマウスがホバーしていたらHoveringTagCDを追加/削除するsystemを登録
    world.system<const SelectableCD>("hovering_tag_update_sys")
      .without(flecs::Prefab)
      .each([&world](flecs::entity e, const SelectableCD& selectable) {
        if (selectable.area.mouseOver()) {
          if (!e.has<HoveringTagCD>()) {
            world.defer([e]() { e.add<HoveringTagCD>(); });
          }
        } else {
          if (e.has<HoveringTagCD>()) {
            world.defer([e]() { e.remove<HoveringTagCD>(); });
          }
        }
      });
  }
};