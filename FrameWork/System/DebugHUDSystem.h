#pragma once
#include <Siv3D.hpp>

#include "FrameWork/Util/FontUtil.h"  // FontUtilのユーティリティを利用
#include "FrameWork/flecs/flecs.h"

// デバッグHUD表示用のシステムクラス
class DebugHUDSystem {
  public:
  static void Register(flecs::world& world) {
    // HUD表示システムを登録
    world.system("debug_hud_sys")
      .kind(flecs::PostFrame)  // フレームの最後に実行
      .run([&world](flecs::iter& it) { Update(world); });
  }

  private:
  static void Update(flecs::world& world) {
    // HUD表示システム

    // 画面左上にHUDを表示
    constexpr int padding = 10;
    constexpr double fontSize = 15;
    constexpr int lineHeight = static_cast<int>(fontSize * 1.4);
    int y = padding;

    // StandardFontをFontUtilで取得
    flecs::entity fontEntity = FontUtil::FindFontCD(world, U"StandardFont");
    const FontSharedCD* fontCD = fontEntity.get<FontSharedCD>();
    if (!fontCD) {
      // フォントが見つからない場合は何も描画しない
      return;
    }
    const s3d::Font& font = fontCD->font;

    // FPS表示
    const int fps = Profiler::FPS();
    const String fpsText = U"FPS: " + Format(fps);
    font(fpsText).draw(fontSize, Arg::topLeft = Vec2(padding, y),
                       Palette::Black);
    y += lineHeight;

    // Profiler::GetStat()で取得できるデバッグ値を表示
    const int drawCalls = Profiler::GetStat().drawCalls;
    // drawCall数を表示
    const String drawCallText = U"Draw Calls: " + Format(drawCalls);
    font(drawCallText)
      .draw(fontSize, Arg::topLeft = Vec2(padding, y), Palette::Black);
    y += lineHeight;

    // ウィンドウサイズ表示
    const Size windowSize = Scene::Size();
    const String sizeText =
      U"Window Size: " + Format(windowSize.x) + U" x " + Format(windowSize.y);
    font(sizeText).draw(fontSize, Arg::topLeft = Vec2(padding, y),
                        Palette::Black);
    y += lineHeight;

    // マウス位置表示
    const Point mousePos = Cursor::Pos();
    const String mouseText =
      U"Mouse Pos: " + Format(mousePos.x) + U", " + Format(mousePos.y);
    font(mouseText).draw(fontSize, Arg::topLeft = Vec2(padding, y),
                         Palette::Black);
    y += lineHeight;

    // その他のHUD情報もここに追加可能
  }
};
// DebugHUDSystem.h
