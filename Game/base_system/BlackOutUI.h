// BlackOutUI.h
#pragma once
#include <Siv3D.hpp>
#include "Game/utility/FontManager.h"

// 暗転処理と暗転時に表示するメッセージを管理するクラス
// モノステートパターンで実装され、どこからでもアクセス可能
class BlackOutUI {
  public:
  // 状態列挙型
  enum class State {
    Hidden,     // 非表示
    FadingIn,   // フェードイン中
    Visible,    // 完全に表示
    FadingOut   // フェードアウト中
  };

  // コンストラクタ/デストラクタ
  BlackOutUI() = delete;
  ~BlackOutUI() = delete;

  // 定数
  static constexpr ColorF kBlackOutColor{0.0, 0.0, 0.0};  // 暗転の色（黒）
  static constexpr int32 kFontSize = 32;                   // メッセージ表示のフォントサイズ
  static constexpr ColorF kTextColor{1.0, 1.0, 1.0};      // テキスト色（白色）
  static constexpr double kDefaultFadeDuration = 1.0;      // デフォルトのフェード時間（秒）

  // 初期化
  static void Initialize() {
    message_.clear();
    fadeAlpha_ = 0.0;
    state_ = State::Hidden;
    fadeDuration_ = kDefaultFadeDuration;
    fadeTimer_.reset();
  }

  // フェードイン開始（徐々に暗くなる）
  static void FadeIn([[maybe_unused]] double duration = kDefaultFadeDuration) {
    state_ = State::FadingIn;
    fadeDuration_ = duration;
    fadeTimer_.restart();
  }

  // フェードアウト開始（徐々に明るくなる）
  static void FadeOut([[maybe_unused]] double duration = kDefaultFadeDuration) {
    state_ = State::FadingOut;
    fadeDuration_ = duration;
    fadeTimer_.restart();
  }

  // メッセージ設定
  static void SetMessage([[maybe_unused]] const String& message) {
    message_ = message;
  }

  // テキスト色を設定（ColorF）
  static void SetTextColor([[maybe_unused]] const ColorF& color) {
    textColor_ = color;
  }

  // テキスト色をデフォルトに戻す
  static void ResetTextColor() {
    textColor_ = kTextColor;
  }

  // メッセージクリア
  static void ClearMessage() {
    message_.clear();
  }

  // 即座に完全表示（フェードなし）
  static void ShowImmediately() {
    state_ = State::Visible;
    fadeAlpha_ = 1.0;
    fadeTimer_.reset();
  }

  // 即座に非表示（フェードなし）
  static void HideImmediately() {
    state_ = State::Hidden;
    fadeAlpha_ = 0.0;
    fadeTimer_.reset();
  }

  // 更新処理
  static void Update() {
    if (state_ == State::FadingIn) {
      const double elapsed = fadeTimer_.sF();
      const double progress = Math::Min(elapsed / fadeDuration_, 1.0);
      fadeAlpha_ = progress;

      if (progress >= 1.0) {
        state_ = State::Visible;
        fadeTimer_.reset();
      }
    } else if (state_ == State::FadingOut) {
      const double elapsed = fadeTimer_.sF();
      const double progress = Math::Min(elapsed / fadeDuration_, 1.0);
      fadeAlpha_ = 1.0 - progress;

      if (progress >= 1.0) {
        state_ = State::Hidden;
        fadeTimer_.reset();
      }
    }
  }

  // 描画処理
  static void Draw() {
    if (state_ == State::Hidden) {
      return;
    }

    // 暗転を描画
    Scene::Rect().draw(ColorF{kBlackOutColor, fadeAlpha_});

    // メッセージがある場合は描画
    if (!message_.isEmpty()) {
      const auto& font = FontManager::GetFont(U"ui_large");
      const Vec2 center = Scene::Center();
        font(message_).draw(Arg::center = center, ColorF{textColor_, fadeAlpha_});
    }
  }

  // フェード中かどうかを返す
  [[nodiscard]] static bool IsFading() noexcept {
    return state_ == State::FadingIn || state_ == State::FadingOut;
  }

  // 完全に表示されているかどうかを返す
  [[nodiscard]] static bool IsVisible() noexcept {
    return state_ == State::Visible;
  }

  // 完全に非表示かどうかを返す
  [[nodiscard]] static bool IsHidden() noexcept {
    return state_ == State::Hidden;
  }

  // 現在のアルファ値を取得
  [[nodiscard]] static double GetAlpha() noexcept {
    return fadeAlpha_;
  }

  private:
  // データメンバ（モノステートパターンのため静的）
  static inline String message_;           // 暗転時に表示するメッセージ
  static inline double fadeAlpha_ = 0.0;   // フェードのアルファ値（0.0～1.0）
  static inline State state_ = State::Hidden;  // 現在の状態
  static inline double fadeDuration_ = kDefaultFadeDuration;  // フェードの所要時間（秒）
  static inline Stopwatch fadeTimer_;      // フェードタイマー
  // 実行時に変更可能なテキスト色（デフォルトは kTextColor）
  static inline ColorF textColor_ = kTextColor;
};
