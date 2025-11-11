// LlamaComponents.h - 共通定義とインクルード
#pragma once
#include <Siv3D.hpp>
#include <memory>
#include <optional>

#include "LlamaConfig.h"
#include "llama.h"

namespace llama_cpp {

// エラー型定義
enum class LlamaError {
  kModelLoadFailed,
  kContextCreateFailed,
  kVocabLoadFailed,
  kSamplerCreateFailed,
  kTokenizationFailed,
  kDecodeFailed
};

// Result型の定義（std::expectedの代替）
template <typename T>
struct Result {
  bool success = false;
  std::optional<T> value = std::nullopt;
  LlamaError error = LlamaError::kModelLoadFailed;

  explicit operator bool() const { return success; }

  T& operator*() { return value.value(); }

  const T& operator*() const { return value.value(); }

  static Result<T> Ok(T&& val) {
    Result<T> result;
    result.success = true;
    result.value = std::move(val);
    return result;
  }

  static Result<T> Error(LlamaError err) {
    Result<T> result;
    result.success = false;
    result.error = err;
    return result;
  }
};

// 初期化結果を表すシンプルな型
struct InitResult {
  bool success = false;
  s3d::String error_message;

  explicit operator bool() const { return success; }
  static InitResult Ok() { return {true, U""}; }
  static InitResult Error(const s3d::String& msg) { return {false, msg}; }
};

}  // namespace llama_cpp