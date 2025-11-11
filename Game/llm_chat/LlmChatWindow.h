#pragma once
#include <Siv3D.hpp>

#include "FrameWork/LlamaCpp/LlamaModel.h"
#include "FrameWork/LlamaCpp/LlamaTextBuffer.h"
#include "FrameWork/LlamaCpp/LlamaTextGenerator.h"
#include "FrameWork/UI/ChatMessageWindow.h"
#include "Game/utility/DebugUtil.h"

// チャットウィンドウ用の設定（UI 関連／LLM の初期プロンプトなど）
// - input_area_height: 入力エリアの高さ（ピクセル）
// - system_prompt: LLM に渡すシステムプロンプト（省略可）
struct LlmChatWindowSetting {
  double input_area_height = 30.0;
  s3d::StringView system_prompt;
  int max_input_char = 100;  // 最大入力文字数
};

// LlmChatWindow の構築に必要なパラメータをまとめた構造体
// - chat_window_desc: 内部で使う ChatMessageWindow の設定
// - setting: LLM や入力エリアに関する設定
struct LlmChatWindowDesc {
  ChatMessageWindowDesc chat_window_desc;
  LlmChatWindowSetting setting;
};

class LlmChatWindow {
  public:
  // コンストラクタ: 描画・メッセージ管理用の ChatMessageWindow と設定を受け取る
  explicit LlmChatWindow(const LlmChatWindowDesc& desc)
      : m_chat_window(desc.chat_window_desc),
        m_setting(desc.setting) {}

  // LLM モデルを渡してジェネレータを初期化する。
  // 成功すれば true を返す（失敗時は false）。アプリ開始時に一度呼ぶ想定。
  bool Initialize(std::shared_ptr<llama_cpp::LlamaModel> model) {
    m_chat_message_generator = initializeChatMessageGenerator(model, m_setting.system_prompt);
    if (!m_chat_message_generator) {
      return false;
    }

    return true;
  }

  // 毎フレーム呼ぶ更新処理。
  // - 描画
  // - ユーザー入力の取得
  // - LLM 応答生成の完了監視
  void Update() {
    // チャット履歴の描画
    m_chat_window.Draw();

    // 生成完了の監視（非同期生成が完了したらコールバック処理）
    if (m_is_waiting_response && m_chat_message_buffer.IsGenerationComplete()) {
      const auto generated_text = m_chat_message_buffer.GetText();
      onResponseComplete(generated_text);
      m_is_waiting_response = false;
    }

    // 入力エリアの配置計算
    auto chat_window_rect = m_chat_window.GetRect();

    auto input_area_pos = chat_window_rect.bottomCenter() + Vec2::Up() * m_setting.input_area_height / 2;
    auto input_area_size = SizeF{chat_window_rect.w, m_setting.input_area_height};
    RectF input_area_rect{input_area_pos - input_area_size / 2, input_area_size};

    // テキスト入力領域（送信ボタン分を除く幅）
    Vec2 input_text_area_size = Vec2(input_area_rect.w - m_setting.input_area_height, input_area_rect.h);
    RectF input_text_area = {input_area_rect.tl(), input_text_area_size};
    // 入力エリアの編集可否を SimpleGUI::TextArea の enabled 引数で制御する
    SimpleGUI::TextBox(m_input_area, input_text_area.tl(), input_text_area.size.x,
                       m_setting.max_input_char, !m_input_area_disabled);

    // 送信ボタン領域（右端正方形）
    RectF send_button_rect = {
      input_text_area.tr(), SizeF{m_setting.input_area_height, input_area_rect.h}
    };

    // 送信ボタンの描画と処理（押されたら LLM にリクエストを投げる）
    if (SimpleGUI::Button(U"▶", send_button_rect.tl(), m_setting.input_area_height,!m_input_area_disabled)) {
      llama_cpp::LlmRequest request;
      request.prompt = m_input_area.text;  // 入力テキストをプロンプトに設定
      startResponse(request);
    }
  }

  // 外部からメッセージ送信完了時のコールバックを登録できる
  void SetOnMessageReceived(std::function<void(s3d::StringView)> on_message_receive) {
    m_on_message_receive = on_message_receive;
  }

  // 外部から「メッセージを送信したとき」に呼ばれるコールバックを登録できる
  // 引数は送信されたメッセージ本文（ビュー）
  void SetOnMessageSent(std::function<void(s3d::StringView)> on_message_sent) {
    m_on_message_sent = on_message_sent;
  }

  // チャット履歴をクリアする
  void Clear() {
    m_chat_window.Clear();
    m_input_area.clear();
  }

  void ClearInputArea() {
    m_input_area.clear();
  }

  // メッセージを直接追加する（LLMを介さない）
  void AddMessage(const s3d::String& text, Sender sender) {
    m_chat_window.AddMessage(text, sender);
  }

  // LLMの応答を待っているかどうかを取得する
  [[nodiscard]] bool IsWaitingForReply() const noexcept {
    return m_is_waiting_response;
  }

  // 入力エリアを無効化／有効化する
  // - disabled == true のとき編集できなくする
  void SetInputAreaDisabled(bool disabled) {
    m_input_area_disabled = disabled;
  }

  private:
  // LlamaTextGenerator の初期化（静的ヘルパ）
  // - model: 既にロードされた LlamaModel
  // - system_prompt: LLM に渡すシステムプロンプト（会話コンテキストの先頭）
  static std::shared_ptr<llama_cpp::LlamaTextGenerator> initializeChatMessageGenerator(
    std::shared_ptr<llama_cpp::LlamaModel> model,
    s3d::StringView system_prompt) {
    // LlamaTextGenerator を作成
    auto generator = std::make_shared<llama_cpp::LlamaTextGenerator>();

    // コンテキスト設定（モデルとトークン窓口の調整）
    llama_cpp::ContextConfig context_config;
    context_config.context_size = 1024;
    context_config.batch_size = 512;
    context_config.threads = 8;
    context_config.threads_batch = 8;

    // サンプリング設定（温度・top_k/top_p 等）
    llama_cpp::SamplingConfig sampling_config;
    sampling_config.temperature = 0.7f;
    sampling_config.top_k = 40;
    sampling_config.top_p = 0.9f;

    // LlamaTextGenerator を初期化。失敗時は nullptr を返す
    auto generator_init_result = generator->InitializeWithModel(
      model, context_config, sampling_config, s3d::String{system_prompt});
    if (!generator_init_result) {
      DebugUtil::Console << U"Failed to initialize LlamaTextGenerator: "
            << generator_init_result.error_message;
      return nullptr;
    }

    return generator;
  }

  // レスポンス生成開始処理（非同期生成を開始する）
  // - バッファをクリアし、ジェネレータに生成を要求する
  // - ユーザーの送信メッセージをチャットに追加し、入力をクリアする
  void startResponse(const llama_cpp::LlmRequest& request) {
    if (!m_chat_message_generator) {
      DebugUtil::Console
        << U"LlamaTextGenerator is not initialized. Call Initialize() first.";
      return;
    }

    m_chat_message_buffer.ClearBuffer();
    m_chat_message_buffer.StartGeneration(*m_chat_message_generator, request);
    m_chat_window.AddMessage(request.prompt, Sender::Self);

    // 送信時コールバックを実行（登録されていれば）
    if (m_on_message_sent) {
      m_on_message_sent(request.prompt);
    }

    m_input_area.clear();
    m_is_waiting_response = true;
  }

  // 生成完了時の共通処理（コールバック通知とチャットへの追加）
  void onResponseComplete(const s3d::String& text) {
    if (m_on_message_receive) {
      m_on_message_receive(text);
    }
    m_chat_window.AddMessage(text, Sender::Partner);
  }

  // メンバ変数群
  llama_cpp::LlamaTextBuffer m_chat_message_buffer;                         // 生成中テキストを蓄えるバッファ
  std::shared_ptr<llama_cpp::LlamaTextGenerator> m_chat_message_generator;  // テキスト生成器（null なら未初期化）

  LlmChatWindowSetting m_setting;  // 表示等の設定

  ChatMessageWindow m_chat_window;                            // メッセージ履歴表示用ウィンドウ
  TextEditState m_input_area;                                 // テキスト入力状態
  bool m_input_area_disabled = false;                         // 入力エリアの編集不可フラグ
  std::function<void(s3d::StringView)> m_on_message_receive;  // 生成完了時の外部通知コールバック
  std::function<void(s3d::StringView)> m_on_message_sent;     // メッセージ送信時の外部通知コールバック
  bool m_is_waiting_response = false;                         // LLM 応答を待っているかのフラグ
};
