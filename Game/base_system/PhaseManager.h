#pragma once
#include <Siv3D.hpp>

#include "Game/utility/PhaseType.h"
#include "Game/utility/iPhase.h"

// PhaseManagerクラス
// フェーズの管理と切り替えを行う
// 循環includeを避けるため、各Phaseの生成処理は外部から登録する方式を採用
class PhaseManager {
  public:
  PhaseManager() = delete;

  // Phase生成関数の型定義
  using PhaseFactory = std::function<std::shared_ptr<iPhase>()>;

  // 初期化（Phase生成関数を登録）
  static void Initialize() {
    phaseFactories_.clear();
    currentPhase_.reset();
    currentPhaseType_ = PhaseType::Introduction;
  }

  // Phase生成関数を登録
  static void RegisterPhaseFactory(PhaseType phaseType, PhaseFactory factory) {
    phaseFactories_[phaseType] = std::move(factory);
  }

  // 更新処理
  static void Update() {
    if (currentPhase_) {
      currentPhase_->update();
    }
  }

  // 描画処理
  static void Draw() {
    if (currentPhase_) {
      currentPhase_->draw();
    }
  }

  static void PostDraw() {
    if (currentPhase_) {
      currentPhase_->postDraw();
    }
  } 

  // フェーズ変更
  static void ChangePhase(PhaseType phaseType) {
    // 登録されたファクトリ関数を検索
    auto it = phaseFactories_.find(phaseType);
    if (it != phaseFactories_.end()) {
      // ファクトリ関数を実行してPhaseを生成
      currentPhase_ = it->second();
      // 現在のフェーズ種別を更新
      currentPhaseType_ = phaseType;
    } else {
      // 登録されていない場合はエラー
      Console << U"PhaseManager::ChangePhase - 未登録のPhaseType: " << static_cast<int32>(phaseType);
      assert(false && "Unregistered PhaseType");
    }
  }

  // 現在の PhaseType を取得
  [[nodiscard]] static PhaseType GetCurrentPhaseType() {
    return currentPhaseType_;
  }

  // 現在のフェーズ名を取得
  [[nodiscard]] static String GetCurrentPhaseName() {
    return phase_type_util::ToString(currentPhaseType_);
  }

  // 現在の次の PhaseType を取得（列挙の順序に従い最後は先頭に戻る）
  [[nodiscard]] static PhaseType GetNextPhaseType() {
    const int count = static_cast<int>(PhaseType::GameClear) + 1;
    int idx = static_cast<int>(currentPhaseType_);
    idx = (idx + 1) % count;
    return static_cast<PhaseType>(idx);
  }

  private:
  static inline std::shared_ptr<iPhase> currentPhase_;                  // 現在のフェーズ
  static inline PhaseType currentPhaseType_ = PhaseType::Introduction;  // 現在のフェーズ種別
  static inline HashTable<PhaseType, PhaseFactory> phaseFactories_;     // PhaseType毎の生成関数
};
