#pragma once
#include <memory>

#include "Game/base_system/SisterMessageUI.h"

// SisterMessageUIを管理するstaticクラス
class SisterMessageUIManager {
public:
  // 初期化
  static void Initialize() {
    sisterMessageUI_ = std::make_shared<SisterMessageUI>();
  }

  // 更新処理
  static void Update() {
    if (sisterMessageUI_) {
      sisterMessageUI_->Update();
    }
  }

  // 描画処理
  static void Draw() {
    if (sisterMessageUI_) {
      sisterMessageUI_->Draw();
    }
  }

  // SisterMessageUIの共有ポインタを取得
  [[nodiscard]] static std::weak_ptr<SisterMessageUI> GetSisterMessageUI() noexcept {
    return sisterMessageUI_;
  }

private:
  // データメンバ
  static inline std::shared_ptr<SisterMessageUI> sisterMessageUI_;  // SisterMessageUIの所有
};
