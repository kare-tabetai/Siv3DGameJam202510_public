// NightDreamPhase.h
#pragma once
#include <Siv3D.hpp>

#include "Game/base_system/BackGroundManager.h"
#include "Game/base_system/BlackOutUI.h"
#include "Game/base_system/MessageWindowUI.h"
#include "Game/base_system/PhaseManager.h"
#include "Game/utility/MouseEffectManager.h"
#include "Game/utility/SoundManager.h"
#include "Game/utility/iPhase.h"

// 夜の夢フェーズを管理するクラス
// 2日目から3日目へ移行する際にのみ発生し、妹との思い出を表示する
class NightDreamPhase : public iPhase {
  public:
  // 状態列挙型
  enum class State {
    FadeInWait,              // 暗転が明けるのを待つ
    MessageDisplayInterval,  // 暗転明けからメッセージ表示までのインターバル
    ShowingMessage,          // メッセージ表示中
    FadeOutInterval,         // メッセージ終了後から暗転開始までのインターバル
    FadingOut,               // 暗転中
    PhaseChangeInterval      // 暗転完了後、フェーズ遷移まで待つ状態
  };

  // 定数
  static constexpr double kFadeDuration = 3.0;            // 暗転/明転の所要時間(秒)
  static constexpr double kMessageDisplayInterval = 2.0;  // 暗転明けからメッセージ表示までの待機時間(秒)
  static constexpr double kFadeOutInterval = 2.0;         // メッセージ終了後から暗転開始までの待機時間(秒)
  static constexpr double kPhaseChangeInterval = 1.5;     // 暗転完了後に待つ秒数(秒)

  // 表示するメッセージ配列
  static inline const Array<String> kDreamMessages = {
    U"...",
    U"妹と遊園地に来ていた。",
    U"歳の離れた妹は煌びやかな世界に夢中で、\n「次はこっち、今度はあっち」とあちこちに引っ張っていくのだった。",
    U"......",
    U"夕暮れになって「最後にメリーゴーランドに乗りたい」と言うのだが、\n疲れ切っていた。",
    U"だから妹を1人でメリーゴーランドに乗せて、見ていることにした。",
    U"その情景がとても、",
    U"とても、大切なもののように感じるのだった。",
    U"...",
    U"......",
    U"........."};

  // コンストラクタ
  explicit NightDreamPhase() {
    // フェーズ開始時は暗転状態から始まるため、暗転が明けるのを待つ状態にする
    currentState_ = State::FadeInWait;

    // 背景とBGMを設定
    BackGroundManager::SetBackGround(U"amusement");
    SoundManager::PlayBGM(U"bgm_amusement_park");

    MouseEffectManager::SetActiveSound(false);

    // 暗転を解除して画面を表示させる(暗転が既にある前提)
    BlackOutUI::FadeOut(kFadeDuration);

    // タイマーを初期化
    intervalTimer_.reset();
  }

  // デストラクタ
  ~NightDreamPhase() override = default;

  // 更新処理
  void update() override {
    // 状態に応じた処理
    switch (currentState_) {
      case State::FadeInWait:
        // 暗転が明けるのを待つ
        if (BlackOutUI::IsHidden()) {
          // 暗転が明けたらインターバルを開始
          currentState_ = State::MessageDisplayInterval;
          intervalTimer_.restart();
        }
        break;

      case State::MessageDisplayInterval:
        // 暗転明けからメッセージ表示までの待機時間
        if (intervalTimer_.sF() >= kMessageDisplayInterval) {
          // 待機時間が経過したらメッセージを表示
          MessageWindowUI::Show(kDreamMessages);
          currentState_ = State::ShowingMessage;
        }
        break;

      case State::ShowingMessage:
        // メッセージ表示完了を待つ
        if (!MessageWindowUI::IsVisible()) {
          // メッセージが終了したらインターバルを開始
          currentState_ = State::FadeOutInterval;
          intervalTimer_.restart();
        }
        break;

      case State::FadeOutInterval:
        // メッセージ終了後から暗転開始までの待機時間
        if (intervalTimer_.sF() >= kFadeOutInterval) {
          // 待機時間が経過したら暗転を開始
          StartFadeOut();
        }
        break;

      case State::FadingOut:
        // 暗転完了を待つ
        if (BlackOutUI::IsVisible()) {
          // 暗転が完了したら、即座にフェーズ遷移せず一定時間待つ状態へ移行する
          currentState_ = State::PhaseChangeInterval;
          intervalTimer_.restart();
        }
        break;

      case State::PhaseChangeInterval:
        // 暗転完了後、定数秒待ってからフェーズを遷移する
        if (intervalTimer_.sF() >= kPhaseChangeInterval) {
          TransitionToNextPhase();
        }
        break;
    }
  }

  // 描画処理
  void draw() const override {
    // MessageWindowUIとBlackOutUIが描画するため、ここでは何もしない
  }

  private:
  // メッセージ表示完了後に暗転を開始する
  void StartFadeOut() {
    currentState_ = State::FadingOut;
    BlackOutUI::FadeIn(kFadeDuration);
  }

  // SunrisePhaseへ遷移する
  void TransitionToNextPhase() {
    BlackOutUI::FadeOut(kFadeDuration);
    PhaseManager::ChangePhase(PhaseType::Sunrise);
  }

  // データメンバ
  State currentState_ = State::FadeInWait;  // 現在の状態
  Stopwatch intervalTimer_;                 // インターバル計測用タイマー(複数の状態で使い回し)
};
