// GameCommonData.h
#pragma once
#include <Siv3D.hpp>

// ゲーム進行中ずっと保持するデータを管理するクラス
// モノステートパターンで実装され、どこからでもアクセス可能
class GameCommonData {
  public:
  // コンストラクタ/デストラクタ
  GameCommonData() = delete;
  ~GameCommonData() = delete;

  // 定数
  static constexpr int32 kInitialMentalPower = 13;  // 初期精神力
  static constexpr int32 kMaxMentalPower = 100;     // 最大精神力
  static constexpr int32 kMinMentalPower = 0;       // 最小精神力
  static constexpr int32 kTotalDays = 3;            // ゲームの総日数
  static inline const Date kStartDate{2031, 6, 20}; // ゲーム開始日

  // 初期化
  static void Initialize() {
    mentalPower_ = kInitialMentalPower;
    currentDay_ = 0;
  }

  // 精神力操作
  static void ChangeMentalPower([[maybe_unused]] int32 delta) {
    mentalPower_ += delta;
    // 範囲制限
    if (mentalPower_ < kMinMentalPower) {
      mentalPower_ = kMinMentalPower;
    }
    if (mentalPower_ > kMaxMentalPower) {
      mentalPower_ = kMaxMentalPower;
    }
  }

  // 日付操作
  static void AdvanceDay() {
    currentDay_++;
  }

  // 精神力取得
  [[nodiscard]] static int32 GetMentalPower() noexcept {
    return mentalPower_;
  }

  // 残り日数取得
  [[nodiscard]] static int32 GetRemainingDays() noexcept {
    return kTotalDays - currentDay_;
  }

  // 現在の経過日数取得
  [[nodiscard]] static int32 GetCurrentDay() noexcept {
    return currentDay_;
  }

  // 精神力が0以下かどうかを判定
  [[nodiscard]] static bool IsMentalPowerZero() noexcept {
    return mentalPower_ <= 0;
  }

  // 最終日かどうかを判定
  [[nodiscard]] static bool IsFinalDay() noexcept {
    return GetRemainingDays() <= 0;
  }

  // 最終日の1日前かどうかを判定
  [[nodiscard]] static bool IsBeforeFinalDay() noexcept {
    return GetRemainingDays() == 1;
  }

  // 初日かどうかを判定
  [[nodiscard]] static bool IsFirstDay() noexcept {
    return currentDay_ == 0;
  }

  // 現在の日付を文字列形式で取得
  [[nodiscard]] static String GetCurrentDateString() {
    const Date currentDate = kStartDate + Days{currentDay_};
    return U"{:04d}/{:02d}/{:02d}"_fmt(currentDate.year, currentDate.month, currentDate.day);
  }

  private:
  // データメンバ（モノステートパターンのため静的）
  static inline int32 mentalPower_ = kInitialMentalPower;  // 精神力
  static inline int32 currentDay_ = 0;                      // 現在の経過日数
};
