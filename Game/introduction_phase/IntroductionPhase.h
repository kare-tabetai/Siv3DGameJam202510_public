// IntroductionPhase.h
#pragma once
#include <Siv3D.hpp>

#include "Game/base_system/BlackOutUI.h"
#include "Game/base_system/PhaseManager.h"
#include "Game/utility/iPhase.h"

// ゲーム開始時の初回のみ実行される導入フェーズを管理するクラス
// 世界観の説明を黒画面に白文字で表示する
class IntroductionPhase : public iPhase {
  public:
  // 状態列挙型
  enum class State {
    WaitingForInitialDelay,  // 初期インターバル待機中
    DisplayingText,          // テキスト表示中
    WaitingForInput,         // ユーザー入力待機中
  };

  // 定数
  static constexpr double kFadeDuration = 1.0;          // フェードイン・アウトの時間（秒）
  static constexpr double kInitialDelayDuration = 1.0;  // 初期インターバル時間（秒）

  // コンストラクタ
  explicit IntroductionPhase() {
    // 最初のテキストを表示
    displayTimer_.start();
    stateTimer_.start();
    BlackOutUI::ShowImmediately();
    MouseEffectManager::SetActiveSound(false);
    state_ = State::WaitingForInitialDelay;
  }

  // デストラクタ
  ~IntroductionPhase() override = default;

  // 更新処理
  void update() override {
    // BlackOutUIの更新は不要（GameManagerで実行される）

    switch (state_) {
      case State::WaitingForInitialDelay:
        // 初期インターバルが経過したらテキスト表示へ遷移
        if (stateTimer_.sF() >= kInitialDelayDuration) {
          state_ = State::WaitingForInput;
          ShowNextText();
          stateTimer_.restart();
        }
        break;
      case State::WaitingForInput:
        // 入力待ち状態でクリックされたら次のテキストへ
        if (MouseL.down()) {
          if (currentTextIndex_ < static_cast<int32>(introductionTexts_.size())) {
            // 次のテキストがあるので表示
            ShowNextText();
          } else {
            BlackOutUI::ClearMessage();
            // すべてのテキストを表示完了したので次のフェーズへ
            TransitionToNextPhase();
          }
        }
        break;
    }
  }

  // 描画処理
  void draw() const override {
    // BlackOutUIが描画を担当するため、ここでは何もしない
  }

  private:
  // 次のテキストをBlackOutUIに設定して表示する
  void ShowNextText() {
    const String& text = introductionTexts_[currentTextIndex_];
    BlackOutUI::SetMessage(text);
    isWaitingForInput_ = false;
            currentTextIndex_++;
  }

  // 次のフェーズ（SunrisePhase）へ遷移する
  void TransitionToNextPhase() {
    // BlackOutUIのフェードアウトをリクエストしてから遷移
    BlackOutUI::FadeOut(kFadeDuration);
    // フェードアウト完了を待つため、少し待機してから遷移
    // 実際のゲームでは次のフレームでフェードアウト完了を確認してから遷移する
    PhaseManager::ChangePhase(PhaseType::Sunrise);
  }

  // データメンバ
  Array<String> introductionTexts_ = {
    U"※このゲームには精神的にショッキングな表現が含まれます。\nご注意ください。",
    U"",
    U"2031年、統合人材AI管理法が施行され、",
    U"すべての企業は、政府中央AI採用システム\n「モノアイ」を通してのみ採用を行えるようになった。"};  // 導入で表示するテキスト配列
  State state_ = State::WaitingForInitialDelay;                                                      // 現在の状態
  int32 currentTextIndex_ = 0;                                                                       // 現在表示中のテキストインデックス
  Stopwatch displayTimer_;                                                                           // テキスト表示時間計測用タイマー
  Stopwatch stateTimer_;                                                                             // 状態管理用タイマー
  bool isWaitingForInput_ = false;                                                                   // プレイヤーの入力待ち状態かどうか
};
