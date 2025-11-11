// SisterMessagePhase.h
#pragma once
#include <Siv3D.hpp>
#include <memory>

#include "Game/base_system/BlackOutUI.h"
#include "Game/base_system/GameCommonData.h"
#include "Game/base_system/MessageWindowUI.h"
#include "Game/base_system/PhaseManager.h"
#include "Game/base_system/SisterMessageUI.h"
#include "Game/base_system/SisterMessageUIManager.h"
#include "Game/utility/DebugUtil.h"
#include "Game/utility/GameConst.h"
#include "Game/utility/SoundManager.h"
#include "Game/utility/iPhase.h"

// 妹とのLINE風チャットを通じて交流するフェーズを管理するクラス
// プレイヤーの入力を受け取り、ローカルLLMを用いて妹AIの返信を生成し、精神力を回復させる
class SisterMessagePhase : public iPhase {
  public:
  enum class State {
    Chatting,                 // チャット中
    BirthDayMessageInterval,  // 誕生日メッセージ表示待ち
    WaitWindowMessage,        // ウィンドウメッセージ表示待ち
    DisplayingWindowMessage,  // ウィンドウメッセージ表示中
    FadeoutWait               // フェーズ終了待ち
  };

  // 定数
  static constexpr int32 kMinMentalRecovery = 3;  // 妹のメッセージによる精神力回復の最小値
  static constexpr int32 kMaxMentalRecovery = 7;  // 妹のメッセージによる精神力回復の最大値

  // ウィンドウメッセージ表示までの遅延（秒）
  static constexpr double kWindowMessageDelaySeconds = 3.0;

  // 誕生日メッセージを表示するまでのインターバル（秒）
  static constexpr double kBirthDayMessageIntervalSeconds = 1.0;

  // コンストラクタ
  explicit SisterMessagePhase() {

    // 部屋系のBGMが再生されていなければ再生
    if (!SoundManager::IsBGMPlaying(U"bgm_room") && !SoundManager::IsBGMPlaying(U"bgm_room_music")) {
    // 室内環境音BGMを再生
      SoundManager::PlayBGM(U"bgm_room");
    }
    BackGroundManager::SetBackGround(U"my_room");

    MouseEffectManager::SetActiveSound(false);

    // UIをアクティブに
    if (auto ui = SisterMessageUIManager::GetSisterMessageUI().lock()) {
      ui->SetActive(true);
    }
  }

  // デストラクタ
  ~SisterMessagePhase() override = default;

  // 更新処理
  void update() override {
    // SisterMessageUIの更新はGameManagerで行われる
    // ここではフェーズ固有のロジックのみ処理

    switch (currentState_) {
      case State::Chatting:
        handleChatting();
        break;
      case State::BirthDayMessageInterval:
        handleBirthDayMessageInterval();
        break;
      case State::WaitWindowMessage:
        handleWaitWindowMessage();
        break;

      case State::DisplayingWindowMessage:
        handleDisplayingWindowMessage();
        break;

      case State::FadeoutWait:
        handleFadeoutWait();
        break;

      default:
        assert(false);
        break;
    }
  }

  // 描画処理
  void draw() const override {
    // SisterMessageUIの描画はGameManagerで行われるため、ここでは何もしない
    // フェーズ固有の描画があればここに追加
  }

  private:
  // 各 State ごとの処理を分離
  void handleChatting() {
    // 既存のチェック処理を使う（メッセージ受信で精神力回復などを行う）
    checkForSisterMessage();
  }

  void handleBirthDayMessageInterval() {
    // 誕生日用のインターバルを StopWatch で計測し、経過したら
    // 誕生日メッセージをセットして通常のウィンドウ表示待ちへ移行する
    if (waitTimer_.sF() >= kBirthDayMessageIntervalSeconds) {
      setupBirthdayMessage();
      // 誕生日メッセージを設定したら、ウィンドウ表示の遅延計測を再スタート
      waitTimer_.restart();
      currentState_ = State::WaitWindowMessage;
    }
  }

  void handleWaitWindowMessage() {
    // Stopwatch を使って遅延を計測する
    if (waitTimer_.sF() >= kWindowMessageDelaySeconds) {
      MessageWindowUI::Show({U"...", U"バイトの時間だ..."});
      currentState_ = State::DisplayingWindowMessage;

      // タイマーをリセット
      waitTimer_.reset();
    }
  }

  void handleDisplayingWindowMessage() {
    // ウィンドウメッセージが完全に閉じるのを待つ
    if (!MessageWindowUI::IsVisible()) {
      // MessageWindowUI が消えたら暗転を開始してフェーズ終了待ちへ移行
      currentState_ = State::FadeoutWait;
      transitionStarted_ = false;  // フェード遷移を未開始にリセット
      BlackOutUI::FadeIn();        // 画面を暗転させる（ブラックアウト）
    }
  }

  void handleFadeoutWait() {
    // 暗転（ブラックアウト）が完了したら一度だけ Work フェーズへ移行しつつ
    // すぐにフェードアウトして画面を戻す
    if (!transitionStarted_) {
      // 完全に暗転（Visible）かつフェード中でないことを確認
      if (BlackOutUI::IsVisible() && !BlackOutUI::IsFading()) {
        transitionStarted_ = true;
        // フェーズ切替
        PhaseManager::ChangePhase(PhaseType::Work);
        // 切替先フェーズ表示のために即座にフェードアウトを開始
        BlackOutUI::FadeOut();

        if (auto ui = SisterMessageUIManager::GetSisterMessageUI().lock()) {
          ui->SetActive(false);
        }
      }
    }
  }

  // 精神力をランダムに3～7回復させる処理
  void recoverMentalPower() {
    const int32 recovery = Random(kMinMentalRecovery, kMaxMentalRecovery);
    GameCommonData::ChangeMentalPower(recovery);
    DebugUtil::Console << U"精神力が" << recovery << U"回復しました。現在の精神力: " << GameCommonData::GetMentalPower();
  }

  void checkForSisterMessage() {
    if (auto ui = SisterMessageUIManager::GetSisterMessageUI().lock()) {
      if (ui->IsMessageReceived()) {
        // 妹からのメッセージを受け取った場合、精神力を回復
        recoverMentalPower();
        // Stopwatchで待機時間を計測開始
        waitTimer_.restart();
        // DisplayingWindowMessage 表示が終わったら暗転して次フェーズへ移る準備をする
        // 実際の暗転開始は MessageWindowUI が閉じられたのを検知してから行う
        // 最終日前日の場合は誕生日メッセージを設定
        if (GameCommonData::IsBeforeFinalDay()) {
          currentState_ = State::BirthDayMessageInterval;
        } else {
          currentState_ = State::WaitWindowMessage;
        }
      }
    }
  }

  // 誕生日メッセージ(最終日前日)を設定する
  void setupBirthdayMessage() {
    if (auto ui = SisterMessageUIManager::GetSisterMessageUI().lock()) {
      const String birthdayMessage =
        U"お兄ちゃん、明日は誕生日だね！！\n"
        U"お祝い送るから楽しみにしててね❤️";
      ui->AddSisterMessage(birthdayMessage);

      // 精神力回復
      recoverMentalPower();
    }
  }

  private:
  State currentState_ = State::Chatting;  // フェーズの現在の状態
  bool transitionStarted_ = false;        // フェード遷移が開始済みかどうか（Work移行の二重呼び出し防止）

  Stopwatch waitTimer_;  // ウィンドウ表示待機に使うタイマー
};
