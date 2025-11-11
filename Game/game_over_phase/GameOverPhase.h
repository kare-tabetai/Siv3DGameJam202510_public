// GameOverPhase.h
#pragma once
#include <Siv3D.hpp>

#include "Game/base_system/BlackOutUI.h"
#include "Game/base_system/MessageWindowUI.h"
#include "Game/utility/GameConst.h"
#include "Game/utility/SoundManager.h"
#include "Game/utility/iPhase.h"

// 精神力が0になった際のゲームオーバーフェーズを管理するクラス
// 正気を失ったような赤文字のセリフとホワイトノイズで辛さを表現する
class GameOverPhase : public iPhase {
  public:
  // 状態列挙型
  enum class State {
    ShowingMessage,    // メッセージ表示中
    FadingOut,         // 暗転中
    WaitingAfterFade,  // 暗転完了後の待機
    WaitLogo,          // ロゴ表示待機
    Finished           // 終了
  };

  // 定数
  static constexpr double kFadeDuration = 1.5;              // 暗転の所要時間（秒）
  static constexpr double kEndMessageDelay = 1.5;           // 暗転完了後にENDを表示するまでの待ち時間（秒）
  static constexpr double kWaitLogoDuration = 3.0;          // ロゴ表示待機から終了までの時間（秒）
  static inline const ColorF kRedTextColor{0.9, 0.1, 0.1};  // 赤文字の色
  static inline const Array<String> kGameOverMessages = {
    U"ア..ア......アアアアアアッァァァァァァァァァ",
    U"もう...何も...かも...",
    U"ダメだ...全て...",
    U"誰にも......必要とされていない.........",
    U"自分は...いる意味がないんだ...",
    U"......"};  // 表示するメッセージ配列

  static inline const String kEndMessage = U"GAME OVER:モノアイはあなたを見ています";

  static constexpr StringView kRedIconPath = U"Asset/monoEye_icon_red.png";  // モノアイのアイコン
  // アイコン描画用の定数
  // 画面幅に対するアンカー位置 (0.5 = 中央, 1/3 = 上から1/3)
  static constexpr Vec2 kRedIconAnchor{0.5, 1.0 / 3.0};
  // 表示サイズ（ピクセル）
  static constexpr double kRedIconDisplaySize{160.0};
  // アイコン描画位置（GameConst::kWindowSize を基準に計算）
  static constexpr Vec2 kRedIconCenter{
    GameConst::kWindowSize.x / 2,
    GameConst::kWindowSize.y * 1.0 / 3.0};

  // コンストラクタ
  explicit GameOverPhase() : redIcon_(kRedIconPath) {
    // ホワイトノイズBGMを再生
    SoundManager::PlayBGM(U"bgm_gameover", SoundManager::kDefaultBGMVolume);

    // ゲームオーバーメッセージをMessageWindowUIに設定
    MessageWindowUI::Show(kGameOverMessages);

    // テキストカラーを赤色に設定
    MessageWindowUI::SetTextColor(kRedTextColor);

    MouseEffectManager::SetActiveSound(false);

    currentState_ = State::ShowingMessage;
    messageCompleted_ = false;
  }

  // デストラクタ
  ~GameOverPhase() override = default;

  // 更新処理
  void update() override {
    // MessageWindowUIの更新
    MessageWindowUI::Update();

    // 状態に応じた処理
    switch (currentState_) {
      case State::ShowingMessage:
        if (MessageWindowUI::IsCompleted() && !messageCompleted_) {
          // メッセージ表示完了を一度だけ検出
          messageCompleted_ = true;

          // 暗転を開始
          StartFadeOut();
        }
        break;

      case State::FadingOut:
        // BlackOutUIの更新
        BlackOutUI::Update();

        // 暗転完了を確認したら待機状態に移行して遅延を開始
        if (BlackOutUI::IsVisible()) {
          // Stopwatch をリスタートして待機状態へ
          stopWatch_.restart();
          currentState_ = State::WaitingAfterFade;
        }
        break;

      case State::WaitingAfterFade:
        // 指定秒数待ってからENDメッセージを表示
        if (stopWatch_.sF() >= kEndMessageDelay) {
          currentState_ = State::WaitLogo;
          BlackOutUI::SetTextColor(kRedTextColor);
          BlackOutUI::SetMessage(kEndMessage);
          // WaitLogo に移行したら経過時間計測をリスタートして
          // 一定時間後に Finished に遷移するようにする
          stopWatch_.restart();
        }
        break;
      case State::WaitLogo:
        // WaitLogo で一定時間経過したら終了状態へ移行
        if (stopWatch_.sF() >= kWaitLogoDuration) {
          currentState_ = State::Finished;
        }

      case State::Finished:
        break;
    }
  }

  // 描画処理
  void draw() const override {
  }

  void postDraw() const override {
    // Finished 状態なら中央上1/3付近に赤いモノアイを表示
    if (currentState_ == State::Finished) {
      // メンバに保持したテクスチャを使用して描画
      // 表示サイズは kRedIconDisplaySize.x を基準に正方形としてリサイズして中央に描画
      redIcon_.resized(kRedIconDisplaySize).draw(Arg::center = kRedIconCenter);
    }
  }

  private:
  // メッセージ表示完了後に暗転を開始する
  void StartFadeOut() {
    MessageWindowUI::Hide();
    BlackOutUI::FadeIn(kFadeDuration);
    currentState_ = State::FadingOut;
  }

  // データメンバ
  State currentState_ = State::ShowingMessage;  // 現在の状態
  bool messageCompleted_ = false;               // メッセージ表示完了フラグ
  Stopwatch stopWatch_;                         // 暗転完了からの経過時間計測用タイマー
  Texture redIcon_;                             // Finished 時に表示する赤アイコンのテクスチャ
};
