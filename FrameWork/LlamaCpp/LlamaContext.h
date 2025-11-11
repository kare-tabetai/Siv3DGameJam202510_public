// LlamaContext.h
#pragma once
#include "LlamaComponents.h"
#include "LlamaModel.h"

namespace llama_cpp {

// RAII wrapper for llama_context
class LlamaContext {
  public:
  // ファクトリーメソッド
  static Result<LlamaContext> Create(const LlamaModel& model,
                                     const ContextConfig& config) {
    if (!model.IsValid()) {
      return Result<LlamaContext>::Error(LlamaError::kModelLoadFailed);
    }

    // コンテキストパラメータの設定
    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = config.context_size;
    ctx_params.n_batch = config.batch_size;
    ctx_params.n_threads = config.threads;
    ctx_params.n_threads_batch = config.threads_batch;

    // コンテキストの作成
    auto context = std::unique_ptr<llama_context, decltype(&llama_free)>(
      llama_init_from_model(model.GetRawModel(), ctx_params), llama_free);

    if (!context) {
      s3d::Console << U"LlamaContext: コンテキストの作成に失敗しました";
      return Result<LlamaContext>::Error(LlamaError::kContextCreateFailed);
    }

    return Result<LlamaContext>::Ok(LlamaContext(std::move(context)));
  }

  // コピー禁止、ムーブ可能
  LlamaContext(const LlamaContext&) = delete;
  LlamaContext& operator=(const LlamaContext&) = delete;
  LlamaContext(LlamaContext&&) = default;
  LlamaContext& operator=(LlamaContext&&) = default;

  // アクセサ
  llama_context* GetRawContext() const { return context_.get(); }
  bool IsValid() const { return context_.get() != nullptr; }

  private:
  explicit LlamaContext(
    std::unique_ptr<llama_context, decltype(&llama_free)> context)
      : context_(std::move(context)) {}

  std::unique_ptr<llama_context, decltype(&llama_free)> context_;
};

}  // namespace llama_cpp
