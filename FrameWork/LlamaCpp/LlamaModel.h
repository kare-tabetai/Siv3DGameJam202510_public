// LlamaModel.h
#pragma once
#include "LlamaComponents.h"

namespace llama_cpp {

// RAII wrapper for llama_model
class LlamaModel {
  public:
  // ファクトリーメソッド
  static Result<LlamaModel> Create(const ModelConfig& config) {
    // バックエンドライブラリの初期化
    ggml_backend_load_all_from_path("./");

    // モデルパラメータの設定
    llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = config.num_gpu_layers;
    model_params.use_mmap = config.use_mmap;
    model_params.use_mlock = config.use_mlock;
    model_params.vocab_only = config.vocab_only;
    model_params.main_gpu = config.main_gpu;

    // モデルの読み込み
    auto model = std::unique_ptr<llama_model, decltype(&llama_model_free)>(
      llama_model_load_from_file(config.model_file_path.narrow().c_str(),
                                 model_params),
      llama_model_free);

    if (!model) {
      s3d::Console << U"LlamaModel: モデルの読み込みに失敗しました - "
                 << config.model_file_path;
      return Result<LlamaModel>::Error(LlamaError::kModelLoadFailed);
    }

    return Result<LlamaModel>::Ok(LlamaModel(std::move(model)));
  }

  // コピー禁止、ムーブ可能
  LlamaModel(const LlamaModel&) = delete;
  LlamaModel& operator=(const LlamaModel&) = delete;
  LlamaModel(LlamaModel&&) = default;
  LlamaModel& operator=(LlamaModel&&) = default;

  // アクセサ
  llama_model* GetRawModel() const { return model_.get(); }
  const llama_vocab* GetVocab() const { return vocab_; }

  bool IsValid() const { return model_ && vocab_; }

  private:
  explicit LlamaModel(
    std::unique_ptr<llama_model, decltype(&llama_model_free)> model)
      : model_(std::move(model)), vocab_(nullptr) {
    if (model_) {
      vocab_ = llama_model_get_vocab(model_.get());
      if (!vocab_) {
        s3d::Console << U"LlamaModel: 語彙の取得に失敗しました";
        model_.reset();  // 無効化
      }
    }
  }

  std::unique_ptr<llama_model, decltype(&llama_model_free)> model_;
  const llama_vocab* vocab_;
};

}  // namespace llama_cpp
