// LlamaModelManager.h
#pragma once
#include <Siv3D.hpp>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "LlamaComponents.h"
#include "LlamaModel.h"

namespace llama_cpp {

// LlamaModelの管理クラス
// 複数のLlamaContextで同じモデルを共有するためのシングルトンマネージャー
class LlamaModelManager {
  public:
  // シングルトンアクセス
  static LlamaModelManager& GetInstance();

  // モデルの初期化（存在しない場合のみ作成）
  InitResult InitializeModel(const s3d::String& model_id,
                             const ModelConfig& config);

  // モデルの取得（共有ポインタ）
  std::shared_ptr<LlamaModel> GetModel(const s3d::String& model_id) const;

  // モデルが初期化済みかチェック
  bool IsModelInitialized(const s3d::String& model_id) const;

  // モデルの解放（明示的な削除）
  void ReleaseModel(const s3d::String& model_id);

  // 全モデルの解放
  void ReleaseAllModels();

  // 初期化済みモデル一覧の取得
  s3d::Array<s3d::String> GetInitializedModelIds() const;

  private:
  // プライベートコンストラクタ（シングルトン）
  LlamaModelManager() = default;
  ~LlamaModelManager() = default;

  // コピー・ムーブ禁止
  LlamaModelManager(const LlamaModelManager&) = delete;
  LlamaModelManager& operator=(const LlamaModelManager&) = delete;
  LlamaModelManager(LlamaModelManager&&) = delete;
  LlamaModelManager& operator=(LlamaModelManager&&) = delete;

  // モデル管理
  mutable std::mutex models_mutex_;
  std::unordered_map<s3d::String, std::shared_ptr<LlamaModel>> models_;
};

// インライン実装
inline LlamaModelManager& LlamaModelManager::GetInstance() {
  static LlamaModelManager instance;
  return instance;
}

inline InitResult LlamaModelManager::InitializeModel(
  const s3d::String& model_id, const ModelConfig& config) {
  std::lock_guard<std::mutex> lock(models_mutex_);

  // 既に初期化済みの場合は成功を返す
  if (models_.find(model_id) != models_.end()) {
#ifdef _DEBUG
    s3d::Console << U"LlamaModelManager: モデル '" << model_id
               << U"' は既に初期化済みです";
               s3d::Print << U"LlamaModelManager: モデル '" << model_id
               << U"' は既に初期化済みです";
#endif
    return InitResult::Ok();
  }

  // モデルの作成
  auto model_result = LlamaModel::Create(config);
  if (!model_result) {
#ifdef _DEBUG
    s3d::Console << U"LlamaModelManager: モデル '" << model_id
               << U"' の作成に失敗しました";
               s3d::Print << U"LlamaModelManager: モデル '" << model_id
               << U"' の作成に失敗しました";
#endif
    return InitResult::Error(U"モデルの作成に失敗しました: " + model_id);
  }

  // 共有ポインタとして管理
  models_[model_id] = std::make_shared<LlamaModel>(std::move(*model_result));

#ifdef _DEBUG
  s3d::Console << U"LlamaModelManager: モデル '" << model_id
             << U"' の初期化が完了しました";
  s3d::Print << U"LlamaModelManager: モデル '" << model_id
             << U"' の初期化が完了しました";
#endif
  return InitResult::Ok();
}

inline std::shared_ptr<LlamaModel> LlamaModelManager::GetModel(
  const s3d::String& model_id) const {
  std::lock_guard<std::mutex> lock(models_mutex_);

  auto it = models_.find(model_id);
  if (it != models_.end()) {
    return it->second;
  }

#ifdef _DEBUG
  s3d::Console << U"LlamaModelManager: モデル '" << model_id
             << U"' が見つかりません";
  s3d::Print << U"LlamaModelManager: モデル '" << model_id
             << U"' が見つかりません";
#endif
  return nullptr;
}

inline bool LlamaModelManager::IsModelInitialized(
  const s3d::String& model_id) const {
  std::lock_guard<std::mutex> lock(models_mutex_);
  return models_.find(model_id) != models_.end();
}

inline void LlamaModelManager::ReleaseModel(const s3d::String& model_id) {
  std::lock_guard<std::mutex> lock(models_mutex_);

  auto it = models_.find(model_id);
  if (it != models_.end()) {
    s3d::Console << U"LlamaModelManager: モデル '" << model_id
               << U"' を解放しました";
    models_.erase(it);
  }
}

inline void LlamaModelManager::ReleaseAllModels() {
  std::lock_guard<std::mutex> lock(models_mutex_);

  s3d::Console << U"LlamaModelManager: 全モデル（" << models_.size()
             << U"個）を解放します";
  models_.clear();
}

inline s3d::Array<s3d::String> LlamaModelManager::GetInitializedModelIds()
  const {
  std::lock_guard<std::mutex> lock(models_mutex_);

  s3d::Array<s3d::String> model_ids;
  model_ids.reserve(models_.size());

  for (const auto& pair : models_) {
    model_ids.push_back(pair.first);
  }

  return model_ids;
}

}  // namespace llama_cpp
