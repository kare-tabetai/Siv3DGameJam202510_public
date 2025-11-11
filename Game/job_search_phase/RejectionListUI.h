// RejectionListUI.h
#pragma once
#include <Siv3D.hpp>

#include "Game/job_search_phase/RejectionInfo.h"
#include "Game/utility/ConfirmDialog.h"
#include "Game/utility/FontManager.h"
#include "Game/utility/SoundManager.h"
#include "Game/utility/UiConst.h"

// 不採用通知のリストを表示するUIクラス
// これまでに受けた全ての不採用情報を履歴書形式で一覧表示し、プレイヤーに「社会から必要とされていない」という精神的苦痛を与える演出を担当する
class RejectionListUI {
  public:
  enum class State {
    InitialLoading,  // リストをひとつづつ表示する演出中
    DialogInterval,  // リストがすべて表示されてから、ダイアログが表示されるまでの待機
    ShowingDialog,   // ダイアログ表示中
  };

  enum class Type {
    Rejection,   // 不採用モード
    Recruitment  // 採用モード
  };

  // 定数

  // 不採用リストウィンドウ自体の定数
  static constexpr double kListSizeWidth = GameConst::kPlayAreaSize.x * 5 / 6;   // 不採用リストの幅
  static constexpr double kListSizeHeight = GameConst::kPlayAreaSize.y * 7 / 8;  // 不採用リストの高さ
  static constexpr double kWindowMargin = 20.0;                                  // ウィンドウの余白
  static constexpr Vec2 kListSize = {kListSizeWidth, kListSizeHeight};           // 不採用リストのサイズ
  static constexpr Vec2 kListCenterPos = GameConst::kPlayAreaCenter;             // 中央位置
  static constexpr Vec2 kListTopLeftPos = kListCenterPos - kListSize / 2;        // 不採用リストの表示位置

  // 不採用リストを表示するエリアの定数
  static constexpr Vec2 kRejectionInfoAreaTopLeft = kListTopLeftPos + Vec2{kWindowMargin, kWindowMargin};
  static constexpr double kRejectionInfoAreaWidth = kListSize.x - kWindowMargin * 2;
  static constexpr double kRejectionInfoAreaHeight = kListSize.y - kWindowMargin * 2;
  static constexpr Vec2 kRejectionInfoAreaSize = {kRejectionInfoAreaWidth, kRejectionInfoAreaHeight};

  // 閉じるボタンの位置とサイズ (リスト基準で定義)
  static constexpr Vec2 kCloseButtonCenterPos = {kListCenterPos.x, kListTopLeftPos.y + kListSize.y + kWindowMargin};
  static constexpr Size kCloseButtonSize = {100, 40};

  static constexpr size_t kRejectionInfoCount = 10;  // 不採用情報の最大表示数
  static constexpr double kRejectionInfoHeight = kRejectionInfoAreaHeight / kRejectionInfoCount;

  static constexpr double kItemHeight = 48.0;        // 1つの不採用情報の高さ
  static constexpr double kItemSpacing = 6.0;        // 不採用情報間の余白(罫線のみ)
  static constexpr double kListPaddingTop = 44.0;    // リスト上部の余白
  static constexpr double kListPaddingLeft = 60.0;   // リスト左側の余白(内側マージン)
  static constexpr double kListPaddingRight = 60.0;  // リスト右側の余白(内側マージン)
  // 演出用の時間定数
  static constexpr double kItemDisplayInterval = 0.18;                    // 不採用情報を一件ずつ表示する間隔(秒)
  static constexpr double kDialogIntervalDuration = 0.8;                  // 全件表示後ダイアログを出すまでの待機時間(秒)
  static constexpr ColorF kBackgroundColor = UiConst::kInputAreaColor;    // リスト内背景色(白系)
  static constexpr ColorF kOuterBackgroundColor = UiConst::kWindowColor;  // リスト外側の背景は共通ウィンドウ色
  static constexpr ColorF kBorderColor{0.0, 0.0, 0.0};                    // 枠線の色(黒)
  static constexpr double kBorderThickness = 1.0;                         // 枠線の太さ
  static constexpr double kCornerRadius = 0.0;                            // 角丸半径(角丸なし)

  // ダイアログのメッセージ
  static inline const String kDialogMessage =
    U"採用判定件数0/100\n本日の採用結果は「全社不採用」となりました。\n明日のご応募をお待ちしています";

  // ダイアログのメッセージ
  static inline const String kRecruiteDialogMessage =
    U"採用判定件数100/100\nおめでとうございます!採用が決定しました。\n後日送付される採用通知をご確認ください。";

  // 不採用リストUIを表示する
  void Show() {
    isVisible_ = true;
    SetUpregections();
    // 初期状態にセットしてタイマーをリセット
    state_ = State::InitialLoading;
    currentRejectionIndex_ = 0;
    stopwatch_.reset();
    stopwatch_.restart();
    // 確認ダイアログは非表示にしておく
    confirmDialog_.Hide();
    type_ = Type::Rejection;
  }

  // 採用リストUIを表示する(全社採用判定時)
  void RecruitShow() {
    isVisible_ = true;
    SetUpRecruitments();
    // 初期状態にセットしてタイマーをリセット
    state_ = State::InitialLoading;
    currentRejectionIndex_ = 0;
    stopwatch_.reset();
    stopwatch_.restart();
    // 確認ダイアログは非表示にしておく
    confirmDialog_.Hide();
    type_ = Type::Recruitment;
  }

  // 毎フレーム呼ばれる更新処理。閉じるボタンのクリック判定を行う
  void Update() {
    if (!isVisible_) {
      return;
    }

    switch (state_) {
      case State::InitialLoading:
        UpdateInitialLoading();
        break;

      case State::DialogInterval:
        UpdateDialogInterval();
        break;

      case State::ShowingDialog:
        UpdateShowingDialog();
        break;

      default:
        break;
    }
  }

  // 毎フレーム呼ばれる描画処理。不採用リストを描画する
  void Draw() const {
    if (!isVisible_) {
      return;
    }

    // リスト背景を描画
    const RectF listRect{kListTopLeftPos, kListSize};
    listRect.rounded(kCornerRadius).draw(kBackgroundColor).drawFrame(kBorderThickness, kBorderColor);

    // 不採用情報を描画
    Vec2 currentPos = kRejectionInfoAreaTopLeft;

    for (size_t i = 0; i < Min(currentRejectionIndex_, rejections_.size()); i++) {
      const RectF itemRect{
        currentPos, SizeF{kRejectionInfoAreaWidth, kRejectionInfoHeight}
      };
      rejections_[i].Draw(itemRect);

      currentPos.y += kRejectionInfoHeight;
    }

    // ダイアログ表示中なら確認ダイアログを上書き描画
    if (state_ == State::ShowingDialog) {
      confirmDialog_.Draw();
    }
  }

  // UIが表示中かどうかを返す
  [[nodiscard]] bool IsVisible() const noexcept {
    return isVisible_;
  }

  // UIが閉じられたかどうかを返す
  [[nodiscard]] bool IsClosed() const noexcept {
    return !isVisible_;
  }

  // UIの状態をリセットする(新しいゲームプレイ時用)
  void Reset() {
    rejections_.clear();
    isVisible_ = false;
    confirmDialog_.Hide();
  }

  private:
  void SetUpregections() {
    rejections_.clear();

    for (size_t i = 0; i < kRejectionInfoCount; ++i) {
      RejectionInfo rejection;
      rejections_.push_back(rejection);
    }
  }

  void SetUpRecruitments() {
    rejections_.clear();

    for (size_t i = 0; i < kRejectionInfoCount; ++i) {
      RejectionInfo recruitment = RejectionInfo::CreateRecruitmentInfo();
      rejections_.push_back(recruitment);
    }
  }

  // 各Stateごとの更新処理を分離
  void UpdateInitialLoading() {
    // 一件ずつ表示する演出: インターバル経過ごとにインデックスを進める
    if (stopwatch_.sF() >= kItemDisplayInterval) {
      // 次のアイテムを表示
      if (currentRejectionIndex_ < rejections_.size()) {
        ++currentRejectionIndex_;
      }

      stopwatch_.restart();

      // すべて表示し終えたら次の状態へ
      if (currentRejectionIndex_ >= rejections_.size()) {
        state_ = State::DialogInterval;
        stopwatch_.restart();
      }
    }
  }

  void UpdateDialogInterval() {
    // 全件表示後、短い待機を置いてダイアログ表示状態へ遷移
    if (stopwatch_.sF() >= kDialogIntervalDuration) {
      state_ = State::ShowingDialog;
      stopwatch_.reset();
      // ShowingDialog へ遷移したら「はい」だけの確認ダイアログを表示
      const String& dialogMessage = (type_ == Type::Recruitment) ? kRecruiteDialogMessage : kDialogMessage;
      confirmDialog_.Show(dialogMessage, kListCenterPos, /*showOnlyYes=*/true, 14);
    }
  }

  void UpdateShowingDialog() {
    confirmDialog_.Update();

    if (confirmDialog_.IsConfirmed()) {
      isVisible_ = false;
    }
  }

  // データメンバ
  Array<RejectionInfo> rejections_;      // 不採用情報のリスト
  bool isVisible_ = false;               // UIが表示中かどうか
  State state_ = State::InitialLoading;  // UIの状態
  size_t currentRejectionIndex_ = 0;     // 現在表示中の不採用情報インデックス
  Type type_ = Type::Rejection;          // 表示タイプ(不採用/採用)
  // 確認ダイアログ
  ConfirmDialog confirmDialog_;
  // 演出用タイマー
  Stopwatch stopwatch_;
};
