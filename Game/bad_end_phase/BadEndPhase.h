// BadEndPhase.h
#pragma once
#include <Siv3D.hpp>
#include <memory>

#include "Game/base_system/BlackOutUI.h"
#include "Game/base_system/MessageWindowUI.h"
#include "Game/base_system/SisterMessageUI.h"
#include "Game/base_system/SisterMessageUIManager.h"
#include "Game/utility/SoundManager.h"
#include "Game/utility/iPhase.h"

// 最終日に不採用となった際のバッドエンドフェーズを管理するクラス
// AI妹ボットサービス終了通知とカーテンのメッセージで、
// 家族（妹）という人間的存在を失う辛さを表現する
class BadEndPhase : public iPhase {
  public:
  // 状態列挙型
  enum class State {
    NotificationInterval, // 通知表示前の短い待機時間（一定時間待機）
    ShowingNotification,  // サービス終了通知表示中
    InputInterval,        // 通知表示前の短い入力間隔（一定時間待機）
    ShowingCurtain,       // カーテンメッセージ表示中
    FadingOut,            // 暗転中
    WaitingToShowEnd,     // 暗転完了後、END表示までの待機中
    Finished              // 終了（暗転完了後、ゲーム終了を待つ）
  };

  // コンストラクタ
  explicit BadEndPhase() {
    BackGroundManager::SetBackGround(U"my_room");
    // 室内環境音BGMを再生
    SoundManager::PlayBGM(U"bgm_room");

    MouseEffectManager::SetActiveSound(false);

    // weak_ptrをlock()してSisterMessageUIを取得
    if (auto sisterUI = SisterMessageUIManager::GetSisterMessageUI().lock()) {
      // サービス終了通知を追加
      sisterUI->SetActive(true);
      sisterUI->SetInputAreaDisabled(true);  // 入力エリアを無効化してメッセージ送信を防止
    }

    inputTimer_.restart();
    currentState_ = State::NotificationInterval;
  }

  // デストラクタ
  ~BadEndPhase() = default;

  // 定数
  static constexpr double kFadeDuration = 6.0;                 // 暗転の所要時間（秒）
  static constexpr double kNotificationIntervalDuration = 4.0;  // 通知表示時間（秒）
  static constexpr double kNotificationDisplayDuration = 8.0;  // 通知表示時間（秒）
  static constexpr double kInputIntervalDuration = 1.0;        // 通知前の入力間隔（秒）
  static constexpr double kEndDisplayDelay = 6.0;              // 暗転完了後に END を表示するまでの待機時間（秒）

  // サービス終了通知文章
  static inline const String kServiceEndNotification =
    U"【重要】AI妹ボットサービス終了のお知らせ\n\n"
    U"平素よりAI妹ボットサービスをご愛顧いただき,ありがとうございます.\n"
    U"このたび,AI妹ボットサービスは,本日2031年6月23日をもって,"
    U"サービスを終了させていただくこととなりました.\n"
    U"ご愛顧いただき,誠にありがとうございました.\n"
    U"AI妹運営チーム一同\n"
    U"2031年6月23日";

  // カーテンメッセージ
  static inline const Array<String> kCurtainMessage = {
    U"...",
    U"......",
    U"........",
  };

  static inline const String kEndMessage = U"END2:一人ぼっち";

  // 毎フレーム呼ばれる更新処理
  void update() override {
    switch (currentState_) {
      case State::NotificationInterval:
        // 通知表示までの短い待機時間が経過したら通知を表示へ移行
        if (inputTimer_.sF() >= kNotificationIntervalDuration) {
          ShowNotification();
          inputTimer_.restart();
          currentState_ = State::InputInterval;
        }
        break;
      case State::InputInterval:
        // コンストラクタ後の短い待機時間が経過したら通知を表示へ移行
        if (inputTimer_.sF() >= kInputIntervalDuration) {
          currentState_ = State::ShowingNotification;
        }
        break;

      case State::ShowingNotification:
        // マウス左クリックで次の状態へ遷移
        if (MouseL.down()) {
          auto sisterUI = SisterMessageUIManager::GetSisterMessageUI().lock();
          if (sisterUI) {
            ShowCurtainMessage();
          }
        }
        break;

      case State::ShowingCurtain:
        // カーテンメッセージ完了を待つ
        if (!MessageWindowUI::IsVisible()) {
          StartFade();
        }
        break;

      case State::FadingOut:
        // 暗転完了を待つ。完全表示になったら END 表示までの待機に移行する
        if (BlackOutUI::IsVisible()) {
          endTimer_.restart();
          currentState_ = State::WaitingToShowEnd;
        }
        break;

      case State::WaitingToShowEnd:
        // 暗転完了後、指定時間経過したら END を表示して終了へ
        if (endTimer_.sF() >= kEndDisplayDelay) {
          BlackOutUI::SetMessage(kEndMessage);
          currentState_ = State::Finished;
        }
        break;

      case State::Finished:
        // ゲーム終了状態（何もしない）
        break;
    }
  }

  // 描画処理
  void draw() const override {
    // GameManagerが所有するSisterMessageUI、MessageWindowUI、BlackOutUIが描画を担当するため、
    // ここではフェーズ固有の描画を行わない
  }

  private:
  // GameManagerからstd::weak_ptr<SisterMessageUI>を取得してlock()し、
  // サービス終了通知を表示する
  void ShowNotification() {

    // weak_ptrをlock()してSisterMessageUIを取得
    if (auto sisterUI = SisterMessageUIManager::GetSisterMessageUI().lock()) {
      // サービス終了通知を追加
      sisterUI->AddSisterMessage(kServiceEndNotification);
    }
  }

  // MessageWindowUI::Show()を呼び出してカーテンのメッセージを表示する
  void ShowCurtainMessage() {
    MessageWindowUI::Show({kCurtainMessage});
    currentState_ = State::ShowingCurtain;
  }

  // メッセージ表示完了後にBlackOutUI::FadeIn()を呼び出して暗転を開始する
  void StartFade() {
    // カーテンメッセージ完了後にG線上のアリアBGMを開始
    SoundManager::PlayBGM(U"bgm_badend");

    currentState_ = State::FadingOut;
    BlackOutUI::FadeIn(kFadeDuration);
  }

  // データメンバ
  State currentState_ = State::NotificationInterval;  // 現在の状態
  Stopwatch inputTimer_;                             // コンストラクタ後の入力間隔タイマー
  Stopwatch endTimer_;                               // END 表示までの待機タイマー
};
