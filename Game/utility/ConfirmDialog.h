// ConfirmDialog.h
#pragma once
#include <Siv3D.hpp>

#include "Game/utility/FontManager.h"
#include "Game/utility/UiConst.h"

// はい/いいえの確認ダイアログを表示するクラス
class ConfirmDialog {
  public:

  // レイアウト関連定数
  static constexpr Vec2 kDefaultDialogSize{400, 200};  // デフォルトのダイアログサイズ
  static constexpr Vec2 kDefaultButtonSize{100, 40};   // デフォルトのボタンサイズ
  // 角丸半径は共通設定を利用する
  static constexpr double kCornerRadius = UiConst::kUiWindowRadius;         // 角丸半径
  static constexpr double kButtonSpacing = 10.0;       // ボタン間のスペース

  // 色関連定数
  // 色関連定数（UiConst の共通色に合わせる）
  static constexpr ColorF kDialogBackgroundColor = UiConst::kWindowColor;      // ダイアログの背景色
  static constexpr ColorF kDialogBorderColor = UiConst::kWindowBorderColor;    // ダイアログの枠線色
  // 半透明のオーバーレイ色は UiConst に定義がないため固定で維持
  static constexpr ColorF kOverlayColor{0.0, 0.0, 0.0, 0.5};                   // 半透明の背景色
  static constexpr ColorF kTextColor = UiConst::kUITextColor;                  // テキストの色
  // ボタンは共通のボタン色を利用し、ホバー時は UiConst のホバー色を利用する
  static constexpr ColorF kYesButtonColor = UiConst::knormalButtonColor;       // 「はい」ボタンの色
  static constexpr ColorF kYesButtonHoverColor = UiConst::khoverButtonColor;   // 「はい」ボタンのホバー色
  static constexpr ColorF kNoButtonColor = UiConst::knormalButtonColor;        // 「いいえ」ボタンの色（共通色を共有）
  static constexpr ColorF kNoButtonHoverColor = UiConst::khoverButtonColor;    // 「いいえ」ボタンのホバー色
  static constexpr ColorF kButtonTextColor = Palette::White;                   // ボタンテキストの色

  // フォント関連定数
  static constexpr int32 kMessageFontSize = 20;  // メッセージのフォントサイズ
  static constexpr int32 kButtonFontSize = 18;   // ボタンのフォントサイズ

  // ダイアログを表示する
  // message: 表示するメッセージ
  // dialogPos: ダイアログの表示位置（中心座標）
  // showOnlyYes: true の場合は「はい」ボタンのみ表示する
  // messageFontSize: メッセージ描画時のフォントサイズ（省略時は kMessageFontSize を使用）
  void Show(const String& message,
            const Vec2& dialogPos = Scene::Center(),
            bool showOnlyYes = false,
            int32 messageFontSize = kMessageFontSize) {
    isVisible_ = true;
    isConfirmed_ = false;
    isCancelled_ = false;
    message_ = message;
    dialogPos_ = dialogPos;
    showOnlyYes_ = showOnlyYes;
    messageFontSize_ = messageFontSize;
  }

  // ダイアログを非表示にする
  void Hide() {
    isVisible_ = false;
  }

  // ダイアログが表示されているかどうかを返す
  [[nodiscard]] bool IsVisible() const noexcept { return isVisible_; }

  // 「はい」ボタンが押されたかどうかを返す
  [[nodiscard]] bool IsConfirmed() const noexcept { return isConfirmed_; }

  // 「いいえ」ボタンが押されたかどうかを返す
  [[nodiscard]] bool IsCancelled() const noexcept { return isCancelled_; }

  // 更新処理（毎フレーム呼ぶ）
  void Update() {
    if (!isVisible_) {
      return;
    }

    // ボタンの位置を計算
    const RectF dialogRect{dialogPos_ - kDefaultDialogSize / 2, kDefaultDialogSize};
    // showOnlyYes_ の場合はボタンを中央に寄せる
    const RectF yesButton = showOnlyYes_
                                ? RectF{dialogRect.center() - Vec2{kDefaultButtonSize.x / 2, -20}, kDefaultButtonSize}
                                : RectF{dialogRect.center() - Vec2{kDefaultButtonSize.x + kButtonSpacing / 2, -20}, kDefaultButtonSize};
    const RectF noButton = RectF{dialogRect.center() - Vec2{-kButtonSpacing / 2, -20}, kDefaultButtonSize};

    // ボタンのクリック判定
    if (yesButton.leftClicked()) {
      isConfirmed_ = true;
      isVisible_ = false;
    }
    if (!showOnlyYes_ && noButton.leftClicked()) {
      isCancelled_ = true;
      isVisible_ = false;
    }
  }

  // 描画処理（毎フレーム呼ぶ）
  void Draw() const {
    if (!isVisible_) {
      return;
    }

    // 半透明の背景を描画
    Scene::Rect().draw(kOverlayColor);

    // ダイアログの描画
    const RectF dialogRect{dialogPos_ - kDefaultDialogSize / 2, kDefaultDialogSize};
    dialogRect.rounded(kCornerRadius)
      .draw(kDialogBackgroundColor)
      .drawFrame(UiConst::kWindowBorderThickness, kDialogBorderColor);

    // メッセージの描画
    const Font& font = FontManager::GetFont(U"ui_medium");
    auto font_pos = dialogRect.center() - Vec2{0, 40};
    font(message_)
      .drawAt(messageFontSize_, font_pos, kTextColor);

    // 「はい」ボタンの描画
    const RectF yesButton{
      dialogRect.center() - Vec2{kDefaultButtonSize.x + kButtonSpacing / 2, -20},
      kDefaultButtonSize
    };
    // はいボタンの位置は showOnlyYes_ の時に中央にする
    const RectF yesButtonPos = showOnlyYes_
                                   ? RectF{dialogRect.center() - Vec2{kDefaultButtonSize.x / 2, -20}, kDefaultButtonSize}
                                   : yesButton;
    const bool yesHovered = yesButtonPos.mouseOver();
    const ColorF yesColor = yesHovered ? kYesButtonHoverColor : kYesButtonColor;
    yesButtonPos.rounded(kCornerRadius)
        .draw(yesColor)
        .drawFrame(UiConst::kWindowBorderThickness, kDialogBorderColor);
    font(U"はい").drawAt(kButtonFontSize, yesButtonPos.center(), kButtonTextColor);

    // 「いいえ」ボタンの描画
    if (!showOnlyYes_) {
      const RectF noButton{
        dialogRect.center() - Vec2{-kButtonSpacing / 2, -20},
        kDefaultButtonSize
      };
      const bool noHovered = noButton.mouseOver();
      const ColorF noColor = noHovered ? kNoButtonHoverColor : kNoButtonColor;
      noButton.rounded(kCornerRadius)
          .draw(noColor)
          .drawFrame(UiConst::kWindowBorderThickness, kDialogBorderColor);
      font(U"いいえ").drawAt(kButtonFontSize, noButton.center(), kButtonTextColor);
    }
  }

  private:
  bool isVisible_ = false;      // ダイアログが表示されているかどうか
  bool isConfirmed_ = false;    // 「はい」ボタンが押されたかどうか
  bool isCancelled_ = false;    // 「いいえ」ボタンが押されたかどうか
  String message_;              // 表示するメッセージ
  Vec2 dialogPos_;              // ダイアログの表示位置
  bool showOnlyYes_ = false;    // true の場合は「はい」ボタンのみ表示する
  int32 messageFontSize_ = kMessageFontSize; // メッセージ描画用フォントサイズ
};
