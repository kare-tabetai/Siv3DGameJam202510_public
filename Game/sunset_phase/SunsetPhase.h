// SunsetPhase.h
#pragma once
#include <Siv3D.hpp>

#include "Game/base_system/BlackOutUI.h"
#include "Game/base_system/GameCommonData.h"
#include "Game/base_system/MessageWindowUI.h"
#include "Game/base_system/PhaseManager.h"
#include "Game/utility/iPhase.h"

// 一日の終わりフェーズを管理するクラス
// 「...もう寝よう」というメッセージを表示し、暗転して次の日へ遷移する
class SunsetPhase : public iPhase {
  public:
  // 状態列挙型
  enum class State {
    FadeInWait,      // 暗転が明けるのを待つ
    ShowingMessage,  // メッセージ表示中
    FadingOut,       // 暗転中
    PhaseChangeInterval  // 暗転完了後、フェーズ遷移まで定数秒待つ状態
  };

  // 定数
  static constexpr double kFadeDuration = 1.5;                            // 暗転の所要時間(秒)
  static constexpr double kPhaseChangeInterval = 0.5;                     // 暗転完了後に待つ秒数(秒)
  static constexpr int32 kNightDreamDay = 1;                              // NightDreamPhaseへ遷移する日数(1日目の終わり)
  static inline const Array<String> kSleepMessages = {U"...もう寝よう"};  // 表示するメッセージ配列

  // コンストラクタ
  explicit SunsetPhase() {
    // フェーズ開始時は暗転状態から始まるため、暗転が明けるのを待つ状態にする
    currentState_ = State::FadeInWait;

    // シーンを設定（メッセージは暗転が明けてから表示）
    BackGroundManager::SetBackGround(U"my_room");
    SoundManager::PlayBGM(U"bgm_room");

    MouseEffectManager::SetActiveSound(false);

    // 暗転を解除して画面を表示させる（暗転が既にある前提）
    BlackOutUI::FadeOut(kFadeDuration);
    // タイマーを初期化
    phaseChangeTimer_.reset();
  }

  // デストラクタ
  ~SunsetPhase() override = default;

  // 更新処理
  void update() override {
    // 状態に応じた処理
    switch (currentState_) {
      case State::FadeInWait:
        // 暗転が明けるのを待つ
        if (BlackOutUI::IsHidden()) {
          // 暗転が明けたらメッセージを表示
          MessageWindowUI::Show(kSleepMessages);
          currentState_ = State::ShowingMessage;
        }
        break;

      case State::ShowingMessage:
        // メッセージ表示完了を待つ
        if (!MessageWindowUI::IsVisible()) {
          StartFadeOut();
        }
        break;

      case State::FadingOut:
        // 暗転完了を待つ
        if (BlackOutUI::IsVisible()) {
          // 暗転が完了したら、即座にフェーズ遷移せず一定時間待つ状態へ移行する
          currentState_ = State::PhaseChangeInterval;
          phaseChangeTimer_.restart();
        }
        break;

      case State::PhaseChangeInterval:
        // 暗転完了後、定数秒待ってからフェーズを遷移する
        if (phaseChangeTimer_.sF() >= kPhaseChangeInterval) {
          TransitionToNextDay();
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

  // GameCommonDataの日数を進め、次のフェーズへ遷移する
  // 2日目の場合はNightDreamPhaseへ、それ以外はSunrisePhaseへ遷移
  void TransitionToNextDay() {
    const int32 currentDay = GameCommonData::GetCurrentDay();
    GameCommonData::AdvanceDay();
    BlackOutUI::FadeOut(kFadeDuration);

    // kNightDreamDayの終わりの場合のみNightDreamPhaseへ遷移
    if (currentDay == kNightDreamDay) {
      PhaseManager::ChangePhase(PhaseType::NightDream);
    } else {
      PhaseManager::ChangePhase(PhaseType::Sunrise);
    }
  }

  // データメンバ
  State currentState_ = State::FadeInWait;  // 現在の状態
  Stopwatch phaseChangeTimer_;               // フェーズ遷移待機用タイマー
};
