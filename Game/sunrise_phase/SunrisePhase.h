// SunrisePhase.h
#pragma once
#include <Siv3D.hpp>

#include "Game/base_system/BackGroundManager.h"
#include "Game/base_system/GameCommonData.h"
#include "Game/base_system/MessageWindowUI.h"
#include "Game/base_system/PhaseManager.h"
#include "Game/utility/iPhase.h"
#include "Game/utility/SoundManager.h"

// 一日の始まりを演出するフェーズクラス
// 「………今日が……始まる。」というメッセージを表示し、次の求職活動フェーズへ遷移する
class SunrisePhase : public iPhase {
  public:
  // 定数

  // 一日の始まりに表示するメッセージ文字列の配列
  static inline const Array<String> kSunriseMessage = {
    U"...",
    U"...今日が......始まる"
  };

  // 1日目に追加で表示するメッセージ
  static inline const Array<String> kFirstDayAdditionalMessage = {
    U"...",
    U"気分転換に曲をかけよう"
  };

  static inline const Array<String> kWakeupDreamMessage = {
    U"...",
    U"...夢...か...",
    U"...",
    U".......",
    U"起きよう...",
  };

  static constexpr int kMusicDay = 1; // 曲をかける日
  static constexpr int kWakeupDreamDay = 2; // 夢を見て目覚めた日

  SunrisePhase() {
    BackGroundManager::SetBackGround(U"my_room");
    SoundManager::PlayBGM(U"bgm_room");

    MouseEffectManager::SetActiveSound(false);
  }

  // デストラクタ
  ~SunrisePhase() override = default;

  // 更新処理
  void update() override {
    // MessageWindowUIの更新は不要(GameManagerで実行される)

    // 完全にフェードアウトしてから動作を始める
    if (BlackOutUI::IsHidden() && !hasShownMessage_) {
      // メッセージウィンドウに表示する内容を初期化
      initializeMessage();
    }

    // 最初のメッセージ表示完了後、1日目なら追加メッセージを表示
    if (hasShownMessage_ && !MessageWindowUI::IsVisible() && !hasCheckedFirstDay_) {
      hasCheckedFirstDay_ = true;
      if (GameCommonData::GetCurrentDay() == kMusicDay && !hasShownAdditionalMessage_) {
        showFirstDayAdditionalMessage();
        return;
      }
    }

    // メッセージ表示中は遷移処理を行わない
    if (hasShownMessage_ && !MessageWindowUI::IsVisible() && hasCheckedFirstDay_) {
      // メッセージ表示完了後、次のフェーズへの遷移をチェック
      checkTransitionToNextPhase();
    }
  }

  // 描画処理
  void draw() const override {
    // CommonUIとMessageWindowUIの描画はGameManagerで実行されるため、ここでは何もしない
  }

  private:
  // メッセージウィンドウに表示する内容を初期化する内部処理
  void initializeMessage() {
    // メッセージを配列として渡す
    if (GameCommonData::GetCurrentDay() == kWakeupDreamDay) {
      MessageWindowUI::Show(kWakeupDreamMessage);
    } else {
      MessageWindowUI::Show(kSunriseMessage);
    }
    hasShownMessage_ = true;
  }

  // 1日目の追加メッセージを表示する内部処理
  void showFirstDayAdditionalMessage() {
    MessageWindowUI::Show(kFirstDayAdditionalMessage);
    hasShownAdditionalMessage_ = true;
    // BGMを切り替え
    SoundManager::PlayBGM(U"bgm_room_music");
  }

  // メッセージ表示完了後、次のフェーズへの遷移をチェックする処理
  void checkTransitionToNextPhase() {
    // メッセージが完了していて、まだ遷移していない場合
    if (hasShownMessage_ && hasCheckedFirstDay_ && !isMessageCompleted_) {
      isMessageCompleted_ = true;
      // JobSearchPhaseへ遷移
      PhaseManager::ChangePhase(PhaseType::JobSearch);
    }
  }

  // データメンバ
  bool hasShownMessage_ = false;            // メッセージの表示を開始したかどうかを示すフラグ
  bool hasCheckedFirstDay_ = false;         // 1日目チェックを完了したかどうかを示すフラグ
  bool hasShownAdditionalMessage_ = false;  // 1日目の追加メッセージを表示したかどうかを示すフラグ
  bool isMessageCompleted_ = false;         // メッセージウィンドウの表示が完了したかどうかを示すフラグ
};
