// LlamaConfig.h
#pragma once
#include <Siv3D.hpp>

#include "llama.h"

namespace llama_cpp {

// モデル設定構造体（モデルファイルの読み込み用）
struct ModelConfig {
  s3d::FilePath model_file_path;  // モデルファイルのパス
  int num_gpu_layers = 0;         // GPU層数
  bool use_mmap = true;           // メモリマップ使用フラグ
  bool use_mlock = false;         // メモリロック使用フラグ
  bool vocab_only = false;        // 語彙のみ読み込みフラグ
  int main_gpu = 0;               // メインGPU番号
};

// コンテキスト設定構造体（コンテキスト初期化用）
struct ContextConfig {
  uint32_t context_size = 4096;  // コンテキストサイズ
  uint32_t batch_size = 512;     // バッチサイズ
  int32_t threads = 0;           // スレッド数（0=自動）
  int32_t threads_batch = 0;     // バッチ処理用スレッド数（0=自動）
};

// サンプリング設定構造体
struct SamplingConfig {
  int top_k = 40;                      // Top-Kサンプリング値
  float top_p = 0.9f;                  // Top-Pサンプリング値
  float temperature = 0.7f;            // 温度パラメータ
  uint32_t seed = LLAMA_DEFAULT_SEED;  // 乱数シード
};

// LLMリクエスト構造体
struct LlmRequest {
  s3d::String prompt;            // 入力プロンプト
  int num_predict_tokens = 128;  // 生成するトークン数
};

}  // namespace llama_cpp