#pragma once
#include <Siv3D.hpp>

#include "CD/PositionCD.h"
#include "CD/Rotation2DCD.h"
#include "CD/ScalarScaleCD.h"
#include "CD/Size2DCD.h"
#include "CD/TextureCD.h"
#include "flecs/flecs.h"

class SpriteRenderingSystem {
  public:
  static void Register(flecs::world& world) {
    // Size2DCDを使用するスプライト描画システム
    auto sprite_size_draw_sys =
      world
        .system<PositionCD, Rotation2DCD, Size2DCD, TextureCD>(
          "sprite_size_draw_sys")
        .kind(flecs::PostFrame)
        .without(flecs::Prefab)
        .each([](PositionCD& pos, Rotation2DCD& rot, Size2DCD& size,
                 TextureCD& texture) {
          // 2Dスプライトとして描画（Z座標は無視）
          Vec2 draw_pos = pos.pos.xy();

          // 2D回転角度を直接取得
          double rotation = rot.rotation;

          // Size2DCDのVec2を直接使用して描画
          texture.tex.resized(size.size).rotated(rotation).drawAt(draw_pos);
        });

    // ScalarScaleCDを使用するスプライト描画システム
    auto sprite_draw_sys =
      world
        .system<PositionCD, Rotation2DCD, ScalarScaleCD, TextureCD>(
          "sprite_draw_sys")
        .kind(flecs::PostFrame)
        .without(flecs::Prefab)
        .each([](PositionCD& pos, Rotation2DCD& rot, ScalarScaleCD& scale,
                 TextureCD& texture) {
          // 2Dスプライトとして描画（Z座標は無視）
          Vec2 draw_pos = pos.pos.xy();

          // 2D回転角度を直接取得
          double rotation = rot.rotation;

          // Texture.drawに直接パラメータを渡して描画
          texture.tex.scaled(scale.scale).rotated(rotation).drawAt(draw_pos);
        });
  }
};
