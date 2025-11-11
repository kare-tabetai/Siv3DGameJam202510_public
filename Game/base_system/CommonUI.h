// CommonUI.h
#pragma once
#include <Siv3D.hpp>

#include "Game/utility/FontManager.h"
#include "Game/utility/GameConst.h"
#include "GameCommonData.h"

// GameCommonDataの要素を画面上に常時表示するUIクラス
// モノステートパターンで実装され、どこからでもアクセス可能
class CommonUI {
  public:
  CommonUI() = delete;

  // 初期化（Scene の幅/高さに依存する値はここで設定する）
  static void Initialize() {}

  // 定数
  static constexpr RectF kBgRect{0, 0, GameConst::kWindowSize.x, GameConst::kCommonUIHeight};  // 背景矩形の位置とサイズ（Initialize()で設定）
  static constexpr ColorF kBgColor{0.0, 0.0, 0.0, 1.0};                                        // 背景色（真っ黒、不透明）
  static constexpr Vec2 kDatePos{20, GameConst::kCommonUIHeight/2};                                                      // 日付表示位置（左上）
  static constexpr Vec2 kRemainingPos{300, GameConst::kCommonUIHeight/2};                                                // 残り日数表示位置（中央寄り上部）
  static constexpr Vec2 kMentalPos = Vec2(GameConst::kWindowSize.x - 200, GameConst::kCommonUIHeight/2);                 // 精神力表示位置（右上、Initialize()で設定）
  static constexpr ColorF kTextColor{1.0, 1.0, 1.0};                                           // テキスト色（白色）
  static constexpr int32 kFontSize = 20;                                                       // フォントサイズ

  // 描画処理
  static void Draw() {
    // 背景を描画
    kBgRect.draw(kBgColor);

    // フォントを取得
    const Font& font = FontManager::GetFont(U"ui_medium");

    // 日付を描画
    const String dateStr = GameCommonData::GetCurrentDateString();
    font(dateStr).draw(Arg::leftCenter =kDatePos, kTextColor);

    // 残り日数を描画
    const int32 remainingDays = GameCommonData::GetRemainingDays();
    const String remainingStr = U"残り日数: {}日"_fmt(remainingDays);
    font(remainingStr).draw(Arg::leftCenter = kRemainingPos, kTextColor);

    // 精神力を描画
    const int32 mentalPower = GameCommonData::GetMentalPower();
    const String mentalStr = U"精神力: {}"_fmt(mentalPower);
    font(mentalStr).draw(Arg::leftCenter = kMentalPos, kTextColor);
  }
};
