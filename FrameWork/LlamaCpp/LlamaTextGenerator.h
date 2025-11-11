// LlamaTextGenerator.h
#pragma once
#include <Siv3D.hpp>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <expected>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "ChatMLUtil.h"
#include "LlamaComponents.h"
#include "LlamaContext.h"
#include "LlamaModel.h"
#include "LlamaModelManager.h"
#include "LlamaSampler.h"

namespace llama_cpp {

// テキスト生成結果
struct GenerationResult {
  bool success = false;
  s3d::String generated_text;
  s3d::String error_message;
};

struct TakeCallBackInfo {
  s3d::String token;           // 生成されたトークン
  s3d::String generated_text;  // これまでに生成されたテキスト
  bool is_end = false;         // 生成終了を示すフラグ
};

using TokenCallBack = std::function<void(const TakeCallBackInfo&)>;

// 非同期テキスト生成クラス
class LlamaTextGenerator {
  public:
  LlamaTextGenerator() = default;

  ~LlamaTextGenerator() {
    CancelAllTasks();
    WaitAllTasks();
  }

  // コピー禁止、ムーブ可能
  LlamaTextGenerator(const LlamaTextGenerator&) = delete;
  LlamaTextGenerator& operator=(const LlamaTextGenerator&) = delete;
  LlamaTextGenerator(LlamaTextGenerator&&) = default;
  LlamaTextGenerator& operator=(LlamaTextGenerator&&) = default;

  // 初期化（既に初期化済みのモデルを使用）
  InitResult InitializeWithModel(std::shared_ptr<LlamaModel> model,
                                 const ContextConfig& context_config,
                                 const SamplingConfig& sampling_config,
                                 const s3d::String& system_prompt = U"") {
    if (!model || !model->IsValid()) {
      return InitResult::Error(U"無効なモデルが指定されました");
    }

    // コンテキストの作成
    auto context_result = LlamaContext::Create(*model, context_config);
    if (!context_result) {
      return InitResult::Error(U"コンテキストの作成に失敗しました");
    }

    // サンプラーの作成
    auto sampler_result = LlamaSampler::Create(sampling_config);
    if (!sampler_result) {
      return InitResult::Error(U"サンプラーの作成に失敗しました");
    }

    // 成功時にコンポーネントを設定
    model_ = model;  // 共有ポインタをそのまま使用
    context_ = std::make_unique<LlamaContext>(std::move(*context_result));
    sampler_ = std::make_unique<LlamaSampler>(std::move(*sampler_result));
    system_prompt_ = system_prompt;  // システムプロンプトを保存

    is_initialized_ = true;
#ifdef _DEBUG
    s3d::Console << U"LlamaTextGenerator: 初期化が完了しました";
    s3d::Print << U"LlamaTextGenerator: 初期化が完了しました";
#endif

    return InitResult::Ok();
  }

  // 同期的テキスト生成
  GenerationResult Generate(const LlmRequest& request) {
    if (!IsInitialized()) {
      return {false, U"", U"初期化されていません"};
    }

    chat_history_.emplace_back(ChatRole::User, request.prompt);

    {
      std::lock_guard<std::mutex> lock(generation_mutex_);

      auto result = RunGeneration(request, nullptr);
      return result;
    }
  }

  // 非同期テキスト生成（トークンごとのコールバック付き）
  void GenerateAsync(const LlmRequest& request,
                     TokenCallBack on_token_callback) {
    if (!IsInitialized()) {
#ifdef _DEBUG
      s3d::Console << U"LlamaTextGenerator: 初期化されていません";
      s3d::Print << U"LlamaTextGenerator: 初期化されていません";
#endif
      return;
    }

    chat_history_.emplace_back(ChatRole::User, request.prompt);

    {
      std::lock_guard<std::mutex> lock(tasks_mutex_);
      // 新しい非同期タスクを開始
      active_tasks_.emplace_back(
        std::async(std::launch::async, [this, request, on_token_callback]() {
          std::lock_guard<std::mutex> generation_lock(generation_mutex_);
          RunGeneration(request, on_token_callback);
        }));
    }
  }

  // すべての非同期タスクの完了を待機
  void WaitAllTasks() {
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    for (auto& task : active_tasks_) {
      if (task.valid()) {
        task.wait();
      }
    }
    active_tasks_.clear();

    // すべてのタスクが終了したのでキャンセルフラグをリセット
    cancel_flag_ = false;
  }

  // 進行中のタスクをキャンセル
  void CancelAllTasks() {
    // キャンセルフラグを設定（実行中のタスクに停止を指示）
    cancel_flag_ = true;

    // 注意: cancel_flag_は WaitAllTasks()
    // でタスクが実際に終了した後にリセットされる
    // ここではフラグの設定のみを行い、実際の待機は WaitAllTasks() に任せる
  }

  // コンテキストをリセット（会話履歴をクリア）
  bool ResetContext() {
    if (!IsInitialized()) {
#ifdef _DEBUG
      s3d::Console << U"LlamaTextGenerator: 初期化されていません";
      s3d::Print << U"LlamaTextGenerator: 初期化されていません";
#endif
      return false;
    }

    {
      std::lock_guard<std::mutex> lock(generation_mutex_);

      // コンテキストのメモリをクリア（会話履歴をリセット）
      llama_memory_t memory = llama_get_memory(context_->GetRawContext());
      llama_memory_clear(memory, true);  // dataも含めてクリア
    }

#ifdef _DEBUG
    s3d::Console << U"LlamaTextGenerator: コンテキストをリセットしました";
#endif
    return true;
  }

  // チャット履歴をクリア
  void ClearChatHistory() {
    std::lock_guard<std::mutex> lock(chat_history_mutex_);
    chat_history_.clear();
#ifdef _DEBUG
    s3d::Console << U"LlamaTextGenerator: チャット履歴をクリアしました";
    s3d::Print << U"LlamaTextGenerator: チャット履歴をクリアしました";
#endif
  }

  // 初期化状態の確認
  bool IsInitialized() const { return is_initialized_; }

  private:
  // 内部的なテキスト生成処理
  GenerationResult RunGeneration(const LlmRequest& request, TokenCallBack on_token_callback) {
    // システムプロンプトがある場合はChatML形式でプロンプトを構築

    s3d::String final_prompt;
    // ChatMLUtilを用いてsystem_prompt_とchat_history_を組み合わせた文字列を作成
    {
      std::lock_guard<std::mutex> lock(chat_history_mutex_);

      // chat_history_をs3d::Arrayに変換
      s3d::Array<ChatMessage> conversation;
      for (const auto& message : chat_history_) {
        conversation.emplace_back(message.role, message.content);
      }

      // ChatML形式でプロンプトを構築
      final_prompt = ChatMLUtil::CreateConversationChatML(system_prompt_, conversation);

      // アシスタントの開始部分を追加（生成開始のため）
      final_prompt += U"\n<|im_start|>assistant\n";
    }

    // プロンプトのトークン化
    const std::string prompt_utf8 = final_prompt.toUTF8();
    int n_prompt = -llama_tokenize(model_->GetVocab(), prompt_utf8.c_str(),
                                   static_cast<int32_t>(prompt_utf8.size()),
                                   nullptr, 0, true, true);

    if (n_prompt <= 0) {
      return {false, U"", U"トークン化に失敗しました"};
    }

    std::vector<llama_token> prompt_tokens(n_prompt);
    if (llama_tokenize(model_->GetVocab(), prompt_utf8.c_str(),
                       static_cast<int32_t>(prompt_utf8.size()),
                       prompt_tokens.data(), n_prompt, true, true) < 0) {
      return {false, U"", U"トークン化処理でエラーが発生しました"};
    }

    llama_batch batch = llama_batch_get_one(
      prompt_tokens.data(), static_cast<int32_t>(prompt_tokens.size()));


    std::string generated_text;
    String token_text;

    // テキスト生成ループ
    for (int n_pos = 0; n_pos + batch.n_tokens < n_prompt + request.num_predict_tokens;) {
      // キャンセル確認
      if (cancel_flag_.load()) {
#ifdef _DEBUG
        s3d::Console << U"テキスト生成がキャンセルされました";
        s3d::Print << U"テキスト生成がキャンセルされました";
#endif
        break;
      }

      // 推論実行
      if (llama_decode(context_->GetRawContext(), batch)) {
        return {false, U"", U"推論処理に失敗しました"};
      }

      n_pos += batch.n_tokens;

      // 次のトークンをサンプリング
      llama_token new_token_id = llama_sampler_sample(
        sampler_->GetRawSampler(), context_->GetRawContext(), -1);

      // 終了トークンチェック
      if (llama_vocab_is_eog(model_->GetVocab(), new_token_id)) {
        break;
      }

      // トークンをテキストに変換
      char buffer[1024];
      int n = llama_token_to_piece(model_->GetVocab(), new_token_id, buffer,
                                   sizeof(buffer), 0, true);

      if (n > 0) {
        generated_text.append(buffer, n);
        token_text += s3d::Unicode::FromUTF8(std::string(buffer, n));

        // コールバック実行
        if (on_token_callback && !token_text.empty()) {
          TakeCallBackInfo info;
          info.token = token_text;
          info.generated_text = s3d::Unicode::FromUTF8(generated_text);
          info.is_end = false;
          on_token_callback(info);

          token_text.clear();
        }
      }

      // 次のバッチを準備
      batch = llama_batch_get_one(&new_token_id, 1);
    }

    // 終了コールバック実行
    if (on_token_callback) {
      TakeCallBackInfo info;
      info.token = U"";
      info.generated_text = s3d::Unicode::FromUTF8(generated_text);
      info.is_end = true;
      on_token_callback(info);
    }

    // 生成されたテキストをチャット履歴に追加
    {
      std::lock_guard<std::mutex> lock(chat_history_mutex_);
      chat_history_.emplace_back(ChatRole::Assistant, s3d::Unicode::FromUTF8(generated_text));
    }

    // パフォーマンス統計出力
    llama_perf_sampler_print(sampler_->GetRawSampler());
    llama_perf_context_print(context_->GetRawContext());

    GenerationResult result = {true, s3d::Unicode::FromUTF8(generated_text), U""};
    return result;
  }

  // コンポーネント（モデルは共有ポインタで管理）
  std::shared_ptr<LlamaModel> model_;
  std::unique_ptr<LlamaContext> context_;
  std::unique_ptr<LlamaSampler> sampler_;

  // システムプロンプト（初期化時に設定）
  s3d::String system_prompt_;

  mutable std::mutex chat_history_mutex_;
  std::vector<ChatMessage> chat_history_;

  // 非同期処理管理
  std::vector<std::future<void>> active_tasks_;
  mutable std::mutex tasks_mutex_;
  std::atomic<bool> cancel_flag_{false};
  std::atomic<bool> is_initialized_{false};

  // 同期処理用ミューテックス（同時実行を防ぐ）
  mutable std::mutex generation_mutex_;
};

}  // namespace llama_cpp
