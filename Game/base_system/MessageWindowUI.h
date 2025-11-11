// MessageWindowUI.h
#pragma once
#include <Siv3D.hpp>

#include "Game/utility/FontManager.h"

// メッセージウィンドウの描画と管理を行うクラス
// モノステートパターンで実装され、どこからでもアクセス可能
class MessageWindowUI {
  public:
  // コンストラクタ/デストラクタ
  MessageWindowUI() = default;
  ~MessageWindowUI() = default;

  // 定数
  static constexpr double kCharDisplayInterval = 0.03;         // 1文字表示間隔（秒）
  static constexpr double kWindowHeightRatio = 0.25;           // ウィンドウの高さ比率（画面の1/4）
  static constexpr double kTextMargin = 30.0;                  // テキストのマージン
  static constexpr ColorF kWindowBgColor{0.0, 0.0, 0.0, 0.9};  // ウィンドウ背景色（濃い半透明の黒）
  static constexpr ColorF kTextColor{1.0, 1.0, 1.0};           // テキスト色（白色）
  static constexpr int32 kFontSize = 24;                       // フォントサイズ
  static constexpr double kMarkerSize = 20.0;                  // 次メッセージマーカー（三角形）のサイズ
  static constexpr double kMarkerAmplitude = 10.0;             // マーカーの上下揺れ幅（ピクセル）
  static constexpr double kMarkerFrequency = 2.0;              // マーカーの揺れ周波数（Hz）

  // 初期化
  static void Initialize() {
    messages_.clear();
    currentIndex_ = 0;
    isVisible_ = false;
    currentCharIndex_ = 0;
    charTimer_.reset();
    textColor_ = kTextColor;

    // ウィンドウ矩形を画面下部に設定
    const double windowHeight = Scene::Height() * kWindowHeightRatio;
    windowRect_ = RectF{0, Scene::Height() - windowHeight, Scene::Width(), windowHeight};
  }

  // メッセージ表示開始
  static void Show(const Array<String>& messages) {
    messages_ = messages;
    currentIndex_ = 0;
    isVisible_ = true;
    currentCharIndex_ = 0;
    charTimer_.restart();
  }

  // メッセージウィンドウを非表示にする
  static void Hide() {
    isVisible_ = false;
    currentIndex_ = 0;
    currentCharIndex_ = 0;
    charTimer_.reset();
  }

  // 次のメッセージへ進む
  static bool Next() {
    if (!isVisible_ || messages_.isEmpty()) {
      return false;
    }

    const String& currentMessage = messages_[currentIndex_];

    // 文字表示中なら全文表示
    if (currentCharIndex_ < static_cast<int32>(currentMessage.length())) {
      currentCharIndex_ = static_cast<int32>(currentMessage.length());
      return true;
    }

    // 次のメッセージへ
    currentIndex_++;
    if (currentIndex_ >= static_cast<int32>(messages_.size())) {
      // 最後のメッセージなので非表示に
      Hide();
      return false;
    }

    // 次のメッセージの表示を開始
    currentCharIndex_ = 0;
    charTimer_.restart();
    return true;
  }

  // 更新処理
  static void Update() {
    if (!isVisible_ || messages_.isEmpty()) {
      return;
    }

    if (clickToAdvance_) {
      // クリックで次へ進む処理
      if (MouseL.down()) {
        Next();
        return;
      }
    }

    // 1文字ずつ表示する処理
    const String& currentMessage = messages_[currentIndex_];
    if (currentCharIndex_ < static_cast<int32>(currentMessage.length())) {
      if (charTimer_.sF() >= kCharDisplayInterval) {
        currentCharIndex_++;
        charTimer_.restart();
      }
    }
  }

  // 描画処理
  static void Draw() {
    if (!isVisible_ || messages_.isEmpty()) {
      return;
    }

    // ウィンドウ背景を描画
    windowRect_.draw(kWindowBgColor);

    // テキストを描画
    const String& currentMessage = messages_[currentIndex_];
    const String displayText = currentMessage.substr(0, currentCharIndex_);

    const Vec2 textPos{windowRect_.x + kTextMargin, windowRect_.y + kTextMargin};
    // const double textWidth = windowRect_.w - kTextMargin * 2;

    const Font& font = FontManager::GetFont(U"ui_medium");
    font(displayText).draw( textPos, textColor_);

    // 全文表示完了時は次メッセージマーカーを表示
    if (currentCharIndex_ >= static_cast<int32>(currentMessage.length())) {
      DrawNextMarker();
    }
  }

  // メッセージが表示中かどうかを返す
  [[nodiscard]] static bool IsVisible() noexcept {
    return isVisible_;
  }

  // すべてのメッセージを表示完了したかどうかを返す
  [[nodiscard]] static bool IsCompleted() noexcept {
    return !isVisible_ && !messages_.isEmpty();
  }

  // テキストカラーを変更する
  static void SetTextColor(const ColorF& color) noexcept {
    textColor_ = color;
  }

  // 現在のテキストカラーを取得する
  [[nodiscard]] static ColorF GetTextColor() noexcept {
    return textColor_;
  }

  // クリックで次へ進む機能の有効/無効を設定する
  static void SetClickToAdvance(bool enabled) noexcept {
    clickToAdvance_ = enabled;
  }

  private:
  // 次メッセージマーカー（三角形）を描画する
  static void DrawNextMarker() {
    // 上下に揺れるアニメーション
    const double offset = Math::Sin(Scene::Time() * Math::TwoPi * kMarkerFrequency) * kMarkerAmplitude;

    // マーカーの位置（ウィンドウ右下）
    const Vec2 markerPos{windowRect_.x + windowRect_.w - kTextMargin - kMarkerSize,
                         windowRect_.y + windowRect_.h - kTextMargin - kMarkerSize + offset};

    // 下向きの三角形を描画
    const Triangle marker{
      markerPos,
      markerPos + Vec2{kMarkerSize, 0},
      markerPos + Vec2{kMarkerSize / 2, kMarkerSize}
    };

    marker.draw(textColor_);
  }

  // データメンバ（モノステートパターンのため静的）
  static inline Array<String> messages_;               // 表示するメッセージ配列
  static inline int32 currentIndex_ = 0;               // 現在表示中のメッセージインデックス
  static inline bool isVisible_ = false;               // 表示中かどうか
  static inline RectF windowRect_;                     // ウィンドウの矩形（画面下1/3）
  static inline int32 currentCharIndex_ = 0;           // 現在表示中の文字インデックス
  static inline Stopwatch charTimer_;                  // 文字表示用タイマー
  static inline ColorF textColor_ = kTextColor;        // テキストの色（デフォルトは白色）
  static inline bool clickToAdvance_ = true;            // クリックで次へ進むかどうか
};
