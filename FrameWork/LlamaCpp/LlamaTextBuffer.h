// LlamaTextBuffer.h
#pragma once
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>

#include "LlamaTextGenerator.h"

namespace llama_cpp {

// スレッドセーフなテキストバッファクラス
class LlamaTextBuffer {
  private:
  // 内部バッファ構造体
  struct BufferData {
    std::mutex text_mutex;      // textアクセス専用
    s3d::String text;
    std::atomic<bool> is_complete{false};    // atomic化でmutex不要
    std::atomic<bool> is_generating{false};  // atomic化でmutex不要
  };

  // shared_ptrで管理し、非同期処理中にオブジェクトが破棄されても安全にアクセス可能にする
  std::shared_ptr<BufferData> buffer_data_ = std::make_shared<BufferData>();

  public:
  LlamaTextBuffer() = default;
  ~LlamaTextBuffer() = default;

  // コピー禁止、ムーブ可能
  LlamaTextBuffer(const LlamaTextBuffer&) = delete;
  LlamaTextBuffer& operator=(const LlamaTextBuffer&) = delete;
  LlamaTextBuffer(LlamaTextBuffer&&) = default;
  LlamaTextBuffer& operator=(LlamaTextBuffer&&) = default;

  // テキストの取得（スレッドセーフ）
  s3d::String GetText() const {
    std::lock_guard<std::mutex> lock(buffer_data_->text_mutex);
    return buffer_data_->text;
  }

  // 非同期生成を開始
  void StartGeneration(LlamaTextGenerator& generator,
                       const LlmRequest& request) {
    // 以前の非同期処理を停止
    generator.CancelAllTasks();
    generator.WaitAllTasks();

    // バッファを初期化
    {
      std::lock_guard<std::mutex> lock(buffer_data_->text_mutex);
      buffer_data_->text.clear();
    }
    buffer_data_->is_complete = false;
    buffer_data_->is_generating = true;

    // 非同期生成を開始（キャプチャでbuffer_data_を共有）
    generator.GenerateAsync(
      request, [buffer_data = buffer_data_](const TakeCallBackInfo& info) {
        {
          std::lock_guard<std::mutex> lock(buffer_data->text_mutex);
          buffer_data->text += info.token;
        }
        // is_endは生成完了を示すフラグ
        if (info.is_end) {
          {
            std::lock_guard<std::mutex> lock(buffer_data->text_mutex);
            buffer_data->text = info.generated_text;
          }
          buffer_data->is_complete = true;
          buffer_data->is_generating = false;
        }
      });
  }

  // バッファをクリア
  void ClearBuffer() {
    {
      std::lock_guard<std::mutex> lock(buffer_data_->text_mutex);
      buffer_data_->text.clear();
    }
    buffer_data_->is_complete = false;
    buffer_data_->is_generating = false;
  }

  // 生成が完了しているか確認
  bool IsGenerationComplete() const { return buffer_data_->is_complete.load(); }

  // テキスト生成中かどうかを確認
  bool IsGenerating() const { return buffer_data_->is_generating.load(); }
};

}  // namespace llama_cpp
