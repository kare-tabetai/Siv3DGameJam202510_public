// LlamaSampler.h
#pragma once
#include "LlamaComponents.h"

namespace llama_cpp {

// RAII wrapper for llama_sampler
class LlamaSampler {
  public:
  // ファクトリーメソッド
  static Result<LlamaSampler> Create(const SamplingConfig& config) {
    auto sampler_params = llama_sampler_chain_default_params();
    sampler_params.no_perf = false;

    auto sampler =
      std::unique_ptr<llama_sampler, decltype(&llama_sampler_free)>(
        llama_sampler_chain_init(sampler_params), llama_sampler_free);

    if (!sampler) {
      s3d::Console << U"LlamaSampler: サンプラーの作成に失敗しました";
      return Result<LlamaSampler>::Error(LlamaError::kSamplerCreateFailed);
    }

    // サンプリング戦略の追加
    llama_sampler_chain_add(sampler.get(),
                            llama_sampler_init_top_k(config.top_k));
    llama_sampler_chain_add(sampler.get(),
                            llama_sampler_init_top_p(config.top_p, 1));
    llama_sampler_chain_add(sampler.get(),
                            llama_sampler_init_temp(config.temperature));
    llama_sampler_chain_add(sampler.get(),
                            llama_sampler_init_dist(config.seed));

    return Result<LlamaSampler>::Ok(LlamaSampler(std::move(sampler)));
  }

  // コピー禁止、ムーブ可能
  LlamaSampler(const LlamaSampler&) = delete;
  LlamaSampler& operator=(const LlamaSampler&) = delete;
  LlamaSampler(LlamaSampler&&) = default;
  LlamaSampler& operator=(LlamaSampler&&) = default;

  // アクセサ
  llama_sampler* GetRawSampler() const { return sampler_.get(); }
  bool IsValid() const { return sampler_.get() != nullptr; }

  private:
  explicit LlamaSampler(
    std::unique_ptr<llama_sampler, decltype(&llama_sampler_free)> sampler)
      : sampler_(std::move(sampler)) {}

  std::unique_ptr<llama_sampler, decltype(&llama_sampler_free)> sampler_;
};

}  // namespace llama_cpp
