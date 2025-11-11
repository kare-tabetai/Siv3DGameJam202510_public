// ConfirmDialog.h
#pragma once
#include <Siv3D.hpp>

#include "Game/utility/FontManager.h"
#include "Game/utility/GameConst.h"
#include "Game/utility/RotatingIcon.h"
#include "Game/utility/UiConst.h"

// はい/いいえの確認ダイアログを表示するクラス
class ServerLoadingUI {
  public:
  // レイアウト関連定数
  static constexpr Vec2 kWindowCenterPos = GameConst::kWindowSize / 2;  // ウィンドウ位置
  static constexpr Vec2 kWindowSize{700, 300};                          // ウィンドウサイズ

  static constexpr Vec2 kTextOffset{0, -20};  // メッセージのオフセット

  // 角丸半径は共通設定を利用する
  static constexpr double kCornerRadius = UiConst::kUiWindowRadius;  // 角丸半径

  // 色関連定数（UiConst の共通色に合わせる）
  static constexpr ColorF kDialogBackgroundColor = UiConst::kWindowColor;    // ダイアログの背景色
  static constexpr ColorF kDialogBorderColor = UiConst::kWindowBorderColor;  // ダイアログの枠線色
  static constexpr ColorF kTextColor = UiConst::kUITextColor;                // テキストの色

  static constexpr double kLoadingIconLogo = 80.0;                      // 下部ロゴの表示サイズ（ピクセル）
  static constexpr Vec2 kLoadingIconLogoOffset{0, kWindowSize.y / 4};  // ウィンドウ中央からの下部ロゴオフセット

  // フォント関連定数
  static constexpr double kMessageFontSize = 20;  // メッセージのフォントサイズ

  ServerLoadingUI(){
    rotatingIcon_.Initialize(kLoadingIconLogo,kWindowCenterPos + kLoadingIconLogoOffset);
  }

  void Show(const String& message) {
    isVisible_ = true;
    message_ = message;
    rotatingIcon_.Show();
  }

  void Hide() {
    isVisible_ = false;
  }

  // ダイアログが表示されているかどうかを返す
  [[nodiscard]] bool IsVisible() const noexcept { return isVisible_; }

  void Update() {
    if (!isVisible_) {
      return;
    }

    // 回転アイコンの更新
    rotatingIcon_.Update();
  }

  // 描画処理（毎フレーム呼ぶ）
  void Draw() const {
    if (!isVisible_) {
      return;
    }

    // WIndowの描画
    const RectF dialogRect{kWindowCenterPos - kWindowSize / 2, kWindowSize};
    dialogRect.rounded(kCornerRadius)
      .draw(kDialogBackgroundColor)
      .drawFrame(UiConst::kWindowBorderThickness, kDialogBorderColor);

    // メッセージの描画
    const Font& font = FontManager::GetFont(U"ui_medium");
    auto font_pos = dialogRect.center() + kTextOffset;
    font(message_).drawAt(font_pos, kTextColor);

    rotatingIcon_.Draw();
  }

  private:
  bool isVisible_ = false;  // ウィンドウが表示されているかどうか
  String message_;          // ウィンドウに表示するメッセージ
  RotatingIcon rotatingIcon_;
};
