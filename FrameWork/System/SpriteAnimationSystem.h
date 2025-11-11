#pragma once
#include <Siv3D.hpp>

#include "CD/FrameCD.h"
#include "CD/SpriteAnimationCD.h"
#include "CD/TextureArrayCD.h"
#include "CD/TextureCD.h"
#include "CD/TimerCD.h"
#include "flecs/flecs.h"

class SpriteAnimationSystem {
  public:
  static void Register(flecs::world& world) {
    // フレーム更新システム: SpriteAnimationCD, TimerCD, TextureArrayCD,
    // FrameCDを持つエンティティを処理
    auto frame_update_sys =
      world
        .system<SpriteAnimationCD, TimerCD, TextureArrayCD, FrameCD>(
          "frame_update_sys")
        .kind(flecs::PreUpdate)
        .without(flecs::Prefab)
        .each([](SpriteAnimationCD& anim, TimerCD& timer,
                 TextureArrayCD& textureArray, FrameCD& frame) {
          // タイマーが間隔を超えた場合、フレームを進める
          if (timer.current_timer >= anim.interval) {
            // タイマーをリセット
            timer.current_timer = 0.0;

            // フレームを進める
            frame.frame++;

            // テクスチャ配列のサイズを超えた場合、ループ処理
            if (frame.frame >= static_cast<int>(textureArray.textures.size())) {
              if (anim.loop) {
                frame.frame = 0;  // ループする場合は0に戻す
              } else {
                frame.frame = static_cast<int>(textureArray.textures.size()) -
                              1;  // ループしない場合は最後のフレームで停止
              }
            }
          }
        });

    // テクスチャ更新システム: TextureArrayCD, TextureCD, SpriteAnimationCD,
    // FrameCDを持つエンティティを処理
    auto texture_update_sys =
      world.system<TextureArrayCD, TextureCD, FrameCD>("texture_update_sys")
        .kind(flecs::PreUpdate)
        .without(flecs::Prefab)
        .with<SpriteAnimationCD>()
        .each(
          [](TextureArrayCD& textureArray, TextureCD& texture, FrameCD& frame) {
            // フレーム番号が有効な範囲内かチェック
            if (frame.frame >= 0 &&
                frame.frame < static_cast<int>(textureArray.textures.size())) {
              // TextureArrayCDの指定されたフレームのテクスチャをTextureCDに設定
              texture.tex = textureArray.textures[frame.frame];
            }
          });
  }
};
