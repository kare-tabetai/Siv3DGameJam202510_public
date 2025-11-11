#pragma once
#include <Siv3D.hpp>
#include <vector>

#include "CD/ColorCD.h"
#include "CD/PositionCD.h"
#include "CD/ScalarScaleCD.h"
#include "CD/TextCD.h"
#include "SharedCD/FontSharedCD.h"
#include "flecs/flecs.h"

class TextUISystem {
  public:
  static void Register(flecs::world& world,
                       const std::vector<flecs::entity>& font_entities) {
    for (size_t i = 0; i < font_entities.size(); ++i) {
      const auto& font_entity = font_entities[i];
      String system_name = U"text_draw_sys_" + ToString(i);

      auto text_draw_sys =
        world
          .system<PositionCD, ScalarScaleCD, TextCD, ColorCD>(
            system_name.narrow().c_str())
          .with(flecs::IsA, font_entity)
          .kind(flecs::PostFrame)
          .without(flecs::Prefab)
          .each([font_entity](PositionCD& pos, ScalarScaleCD& scale,
                              TextCD& text, ColorCD& color) {
            const auto& font = font_entity.get<FontSharedCD>().font;
            font(text.text).draw(scale.scale, pos.pos.xy(), color.c);
          });
    }
  }
};
