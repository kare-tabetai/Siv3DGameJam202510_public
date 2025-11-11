// PasswordInputUI.h
#pragma once
#include <Siv3D.hpp>

#include "Game/utility/FontManager.h"
#include "Game/utility/UiConst.h"

// 求職活動フェーズの冒頭で表示される、モノアイサーバーへのパスワード入力演出を行うUIクラス
// 実際の操作はできず、自動でパスワードが入力されていく様子を演出する
class PasswordInputUI {
  public:
  // UIの状態
  enum class State {
    Display,        // 表示フェーズ
    PasswordInput,  // パスワード入力演出フェーズ
    InputWait,      // 入力完了後、少し待ってから終了するフェーズ
    Completed       // 終了(このUIを閉じる)
  };

  // コンストラクタ
  PasswordInputUI() : iconTexture_(kMonoEyeIconPath) {}

  // 定数
  static constexpr Vec2 kWindowPos{260, 160};                                            // ウィンドウ左上位置
  static constexpr Size kWindowSize{800, 420};                                           // ウィンドウサイズ
  static constexpr Vec2 kWindowCenter = kWindowPos + kWindowSize / 2;                    // ウィンドウ中心位置
  static inline const String kPasswordText = U"●●●●●●●●●●";                              // 表示するパスワード
  static constexpr double kCharInputInterval = 0.25;                                     // 1文字あたりの入力間隔(秒)
  static constexpr double kDisplayDuration = 1.8;                                        // 表示フェーズの表示時間(秒)
  static constexpr double kInputWaitDuration = 1.5;                                      // 入力完了後の待機時間(秒)
  static constexpr Vec2 kIconPos = {kWindowCenter.x, kWindowPos.y + kWindowSize.y / 3};  // 六角形アイコンの表示位置
  static constexpr Vec2 kCitizenNumberLabelPos{420, 450};                                // 「国民番号:」ラベルの表示位置
  static constexpr Vec2 kCitizenNumberInputPos{570, 440};                                // 国民番号入力欄の表示位置
  static constexpr Vec2 kPasswordLabelPos{390, 520};                                     // 「パスワード:」ラベルの表示位置
  static constexpr Vec2 kPasswordInputPos{570, 510};                                     // パスワード入力欄の表示位置
  static constexpr double kInputTextPadding = 3.0;                                       // 入力欄内のテキストのパディング
  static inline const String kCitizenNumber = U"KU1623142";                              // 表示する国民番号(固定値)
  static constexpr Size kInputBoxSize{400, 50};                                          // 入力欄のサイズ
  static constexpr int32 kLabelFontSize = 28;                                            // ラベル表示のフォントサイズ
  static constexpr int32 kInputFontSize = 32;                                            // 入力欄のフォントサイズ

  static constexpr ColorF kTextColor = UiConst::kUITextColor;                       // テキストの色（濃い青）
  static constexpr ColorF kInputBoxColor = UiConst::kInputAreaColor;                // 入力欄の背景色（明るいグレー）
  static constexpr ColorF kInputBoxBorderColor = UiConst::kInputAreaBorderColor;    // 入力欄の枠線色（青みがかったグレー）
  static constexpr double kInputBoxBorderThickness = UiConst::kInputAreaThickness;  // 入力欄の枠線の太さ
  static constexpr double kInputBoxRadius = UiConst::kInputAreaRadius;              // 入力欄の角丸半径（角丸なし）
  static constexpr double kIconSize = 180.0;                                        // 六角形アイコンのサイズ（大きめに）
  static constexpr StringView kMonoEyeIconPath = UiConst::kMonoEyeIconPath;         // モノアイアイコンのアセット

  static constexpr ColorF kWindowColor = UiConst::kWindowColor;                      // ウィンドウ背景色（薄いグレー）
  static constexpr ColorF kWindowBorderColor = UiConst::kWindowBorderColor;          // ウィンドウ枠の色
  static constexpr double kWindowBorderThickness = UiConst::kWindowBorderThickness;  // ウィンドウ枠の太さ
  static constexpr double kWindowRadius = UiConst::kUiWindowRadius;                  // ウィンドウの角丸半径

  // パスワード入力演出を開始する。タイマーをリセットして演出状態を初期化する
  void Show() {
    displayPassword_.clear();
    currentCharIndex_ = 0;
    currentState_ = State::Display;
    stateTimer_ = Timer{SecondsF(kDisplayDuration), StartImmediately::Yes};
    inputTimer_ = Timer{SecondsF(kCharInputInterval)};
    // waitTimer_はInputWaitに遷移したタイミングで開始するためここではリセットしておく
    waitTimer_ = Timer{};
  }

  // 毎フレーム呼ばれる更新処理。
  void Update() {
    switch (currentState_) {
      case State::Display:
        UpdateDisplayPhase();
        break;
      case State::PasswordInput:
        // タイマーに応じて文字を1つずつ追加していく
        UpdatePasswordInputPhase();
        break;
      case State::InputWait:
        // 入力完了後の待機フェーズ
        UpdateInputWaitPhase();
        break;
      case State::Completed:
        // 終了状態では何もしない
        break;
    }
  }

  // 毎フレーム呼ばれる描画処理。パスワード入力画面を描画する
  void Draw() const {
    // 背景ウィンドウ（ザブトン）を描画
    const RectF windowRect{kWindowPos, kWindowSize};
    windowRect.rounded(kWindowRadius).draw(kWindowColor).drawFrame(kWindowBorderThickness, kWindowBorderColor);

    // モノアイアイコンを描画
    iconTexture_.resized(kIconSize).draw(Arg::center = kIconPos);

    // 入力欄の背景と枠線を描画
    constexpr RectF citizenNumberBox{kCitizenNumberInputPos, kInputBoxSize};
    constexpr RectF passwordBox{kPasswordInputPos, kInputBoxSize};

    citizenNumberBox.rounded(kInputBoxRadius).draw(kInputBoxColor).drawFrame(kInputBoxBorderThickness, kInputBoxBorderColor);
    passwordBox.rounded(kInputBoxRadius).draw(kInputBoxColor).drawFrame(kInputBoxBorderThickness, kInputBoxBorderColor);

    // 入力内容を描画
    const Font& inputFont = FontManager::GetFont(U"ui_large");

    constexpr Vec2 citizenNumberTextPos = citizenNumberBox.leftCenter() + Vec2{kInputTextPadding, 0};
    inputFont(kCitizenNumber).draw(kInputFontSize, Arg::leftCenter = citizenNumberTextPos, kTextColor);

    constexpr Vec2 passwordTextPos = passwordBox.leftCenter() + Vec2{kInputTextPadding, 0};
    inputFont(displayPassword_).draw(kInputFontSize, Arg::leftCenter = passwordTextPos, kTextColor);

    // ラベルを描画
    const Font& labelFont = FontManager::GetFont(U"ui_medium");
    labelFont(U"国民番号:").draw(kLabelFontSize, kCitizenNumberLabelPos, kTextColor);
    labelFont(U"パスワード:").draw(kLabelFontSize, kPasswordLabelPos, kTextColor);
  }

  // 演出が完了したかどうかを返す
  [[nodiscard]] bool IsCompleted() const noexcept {
    return currentState_ == State::Completed;
  }

  // 現在の状態を取得する
  [[nodiscard]] State GetState() const noexcept {
    return currentState_;
  }

  private:
  // 表示フェーズの更新処理
  void UpdateDisplayPhase() {
    if (stateTimer_.reachedZero()) {
      // 表示フェーズ終了、パスワード入力演出フェーズへ遷移
      currentState_ = State::PasswordInput;
      inputTimer_ = Timer{SecondsF(kCharInputInterval), StartImmediately::Yes};
    }
  }

  // パスワード入力演出フェーズの更新処理
  void UpdatePasswordInputPhase() {
    // タイマーが経過したら次の文字を入力
    if (inputTimer_.reachedZero()) {
      InputNextChar();

      // 全文字入力完了チェック
      if (currentCharIndex_ >= kPasswordText.size()) {
        // 入力演出終了、InputWaitフェーズへ遷移して少し待ってからCompletedへ移行する
        currentState_ = State::InputWait;
        waitTimer_ = Timer{SecondsF(kInputWaitDuration), StartImmediately::Yes};
      } else {
        // 次の文字入力のためにタイマーをリセット
        inputTimer_.restart();
      }
    }
  }

  // InputWaitフェーズの更新処理: 待機時間経過でCompletedへ遷移
  void UpdateInputWaitPhase() {
    if (waitTimer_.isStarted() && waitTimer_.reachedZero()) {
      currentState_ = State::Completed;
      // Enterの入力音を再生
      SoundManager::PlaySE(U"se_keyboard4");
    }
  }
  // 次の文字を入力する内部処理。効果音の再生も行う
  void InputNextChar() {
    if (currentCharIndex_ < kPasswordText.size()) {
      displayPassword_ += kPasswordText[currentCharIndex_];
      currentCharIndex_++;
      PlayTypingSound();
    }
  }

  // キーボード打鍵音をランダムに再生する
  void PlayTypingSound() {
    const Array<String> typingSounds = {U"se_keyboard1", U"se_keyboard2", U"se_keyboard3"};
    const String selectedSound = typingSounds[Random<size_t>(0, typingSounds.size() - 1)];
    SoundManager::PlaySE(selectedSound);
  }

  // データメンバ
  String displayPassword_;               // 現在表示されているパスワード文字列
  Timer inputTimer_;                     // 文字入力のタイミングを制御するタイマー
  Timer stateTimer_;                     // 状態遷移のタイミングを制御するタイマー
  Timer waitTimer_;                      // 入力完了後の待機タイマー
  size_t currentCharIndex_ = 0;          // 現在入力中の文字インデックス
  State currentState_ = State::Display;  // 現在の状態
  Texture iconTexture_;                  // アイコンテクスチャ
};
