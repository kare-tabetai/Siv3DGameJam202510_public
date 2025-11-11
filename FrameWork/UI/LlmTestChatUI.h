#pragma once
#include <Siv3D.hpp>
#include <memory>

#include "../LlamaCpp/LlamaTextBuffer.h"
#include "../LlamaCpp/LlamaTextGenerator.h"
#include "../LlamaCpp/LlamaModel.h"

// LLMテストチャットUIを管理するクラス
class LlmTestChatUI {
  public:
  LlmTestChatUI() = default;
  ~LlmTestChatUI() = default;

  // 初期化処理（モデルとシステムプロンプトを受け取ってLlamaTextGeneratorを作成）
  bool Initialize(std::shared_ptr<llama_cpp::LlamaModel> model, const String& system_prompt) {
    // LlamaTextGeneratorを作成
    m_llama_generator = std::make_shared<llama_cpp::LlamaTextGenerator>();

    // コンテキスト設定
    llama_cpp::ContextConfig context_config;
    context_config.context_size = 2048;
    context_config.batch_size = 512;
    context_config.threads = 8;
    context_config.threads_batch = 8;

    // サンプリング設定
    llama_cpp::SamplingConfig sampling_config;
    sampling_config.temperature = 0.7f;
    sampling_config.top_k = 40;
    sampling_config.top_p = 0.9f;

    // LlamaTextGeneratorを初期化
    auto generator_init_result = m_llama_generator->InitializeWithModel(model, context_config, sampling_config, system_prompt);
    if (!generator_init_result) {
      Console << U"Failed to initialize LlamaTextGenerator: " << generator_init_result.error_message;
      return false;
    }

    // システムプロンプトを保存
    m_system_prompt = system_prompt;

    return true;
  }

  // レスポンス生成開始処理（LlmRequestでレスポンス生成を開始）
  void StartResponse(const llama_cpp::LlmRequest& request) {
    if (!m_llama_generator) {
      Console << U"LlamaTextGenerator is not initialized. Call Initialize() first.";
      return;
    }

    m_llm_response.ClearBuffer();
    m_llm_response.StartGeneration(*m_llama_generator, request);
  }

  // UI全体の更新処理
  void Update() {
    // Sendボタンの処理
    if (SimpleGUI::Button(U"Send", Vec2{40, 40}, 100,
                          TextInput::GetEditingText().isEmpty())) {
      if (m_llama_generator) {
        llama_cpp::LlmRequest send_request;
        send_request.num_predict_tokens = 256;
        send_request.prompt = m_textAreaEditState.text;
        StartResponse(send_request);
      }
    }

    // Resetボタンの処理
    if (SimpleGUI::Button(U"Reset", Vec2{150, 40}, 100)) {
      // 進行中のLLM生成をキャンセル
      if (m_llama_generator) {
        m_llama_generator->CancelAllTasks();
        m_llama_generator->WaitAllTasks();

        // LLMのコンテキストの情報もリセット
        m_llama_generator->ResetContext();
      }

      // 応答テキストをクリア
      m_llm_response.ClearBuffer();
    }

    // LLM入力テキストエリア
    SimpleGUI::TextArea(m_textAreaEditState, Vec2{40, 90}, SizeF{380, 300},
                        SimpleGUI::PreferredTextAreaMaxChars);

    // LLM応答テキストを画面右側に表示
    DrawResponseArea();
  }

  private:
  TextAreaEditState m_textAreaEditState;
  llama_cpp::LlamaTextBuffer m_llm_response;
  std::shared_ptr<llama_cpp::LlamaTextGenerator> m_llama_generator;
  String m_system_prompt;

  // LLM応答テキストエリアの描画
  void DrawResponseArea() {
    // 背景の描画
    auto llm_text_area = Rect(460, 90, 380, 300);
    llm_text_area.draw(Color(240, 240, 240)).drawFrame(1, Color(200, 200, 200));

    // LLM応答テキストの取得と表示
    String response_text = m_llm_response.GetText();
    if (not response_text.isEmpty()) {
      // SimpleGUIと同じフォントサイズで範囲制限付きで描画
      const auto& simple_gui_font = SimpleGUI::GetFont();
      simple_gui_font(response_text).draw(llm_text_area, Color(50, 50, 50));
    }
  }
};
