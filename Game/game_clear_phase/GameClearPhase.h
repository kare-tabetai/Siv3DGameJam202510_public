// GameClearPhase.h
#pragma once
#include <Siv3D.hpp>
#include "Game/base_system/BlackOutUI.h"
#include "Game/base_system/MessageWindowUI.h"
#include "Game/utility/iPhase.h"

// ゲームクリア時のエンディングフェーズを管理するクラス
// 採用が決まった後の主人公の辟易したセリフを表示し、暗転して終了する
class GameClearPhase : public iPhase {
  public:
  // 状態列挙型
  enum class State {
    WaitingBeforeMessage,  // メッセージ表示前の待機
    ShowingMessage,        // メッセージ表示中
    FadingOut,             // 暗転中
    WaitingAfterFade,      // 暗転完了後の待機
    Finished               // 終了
  };

  // 定数
  static constexpr double kInitialDelay = 0.5;  // メッセージ表示前の待機時間(秒)
  static constexpr double kFadeDuration = 2.0;  // 暗転の所要時間(秒)
  static constexpr double kEndMessageDelay = 1.5; // 暗転完了後にGAME CLEARを表示するまでの待ち時間(秒)
  static inline const Array<String> kEndingMessages = {
      U"...",
      U"やっと終わったか...",
      U"これで、また働ける...",
      U"..."};  // 表示するメッセージ配列

  static inline String kEndMessage = U"END1:社会の歯車";  // 暗転上に表示するメッセージ

  // コンストラクタ
  explicit GameClearPhase()  {
    // 室内環境音BGMを再生
    SoundManager::PlayBGM(U"bgm_room");

    MouseEffectManager::SetActiveSound(false);

    // 待機時間計測用のStopwatchを開始
    initialDelayStopwatch_.start();
    currentState_ = State::WaitingBeforeMessage;
  }

  // デストラクタ
  ~GameClearPhase() override = default;

  // 更新処理
  void update() override {
    // MessageWindowUIの更新
    MessageWindowUI::Update();

    // 状態に応じた処理
    switch (currentState_) {
      case State::WaitingBeforeMessage:
        // 指定時間経過したらメッセージを表示開始
        if (initialDelayStopwatch_.sF() >= kInitialDelay) {
          MessageWindowUI::Show(kEndingMessages);
          currentState_ = State::ShowingMessage;
        }
        break;

      case State::ShowingMessage:
        if (MessageWindowUI::IsCompleted()) {
          // すべてのメッセージを表示完了したら暗転開始
          StartFadeOut();
        }
        break;

      case State::FadingOut:

        // 暗転完了を確認したら待機状態に移行して遅延を開始
        if (BlackOutUI::IsVisible()) {
          // 現在時刻を記録して待機状態へ
          fadeCompleteTime_ = Scene::Time();
          currentState_ = State::WaitingAfterFade;
        }
        break;

      case State::WaitingAfterFade:
        // 指定秒数待ってから GAME CLEAR を表示
        if ((Scene::Time() - fadeCompleteTime_) >= kEndMessageDelay) {
          currentState_ = State::Finished;
          BlackOutUI::SetMessage(kEndMessage);
        }
        break;

      case State::Finished:
        // 暗転上に表示するメッセージを確実に保持
        BlackOutUI::SetMessage(kEndMessage);
        break;
    }
  }

  // 描画処理
  void draw() const override {
    // MessageWindowUIとBlackOutUIが自動的に描画されるため、ここでは何もしない
  }

  private:
  // メッセージ表示完了後に暗転を開始する
  void StartFadeOut() {
    MessageWindowUI::Hide();
    SoundManager::PlayBGM(U"bgm_clear");
    BlackOutUI::FadeIn(kFadeDuration);
    currentState_ = State::FadingOut;
  }

  // データメンバ
  State currentState_ = State::WaitingBeforeMessage;  // 現在の状態
  Stopwatch initialDelayStopwatch_;                   // メッセージ表示前の待機時間計測用
  double fadeCompleteTime_ = 0.0;                     // 暗転完了時刻の記録(Scene::Time())
};
