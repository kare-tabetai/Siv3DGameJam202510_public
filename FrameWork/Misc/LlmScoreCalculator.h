// LlmScoreCalculator.h
// LLMを使用してテキストを評価し、スコアを計算するクラス
#pragma once
#include <Siv3D.hpp>
#include <mutex>
#include <vector>

#include "LlamaCpp/LlamaModel.h"
#include "LlamaCpp/LlamaTextBuffer.h"
#include "LlamaCpp/LlamaTextGenerator.h"

// スコア情報を格納する構造体
struct Score {
  s3d::String evaluated_text;  // 評価されたテキスト
  int score_value;              // スコア値
  s3d::DateTime timestamp;      // 評価時刻
  double calculation_time_ms;   // 計算にかかった時間(ミリ秒)
};

// LlmScoreCalculatorクラス
class LlmScoreCalculator {
  public:
  LlmScoreCalculator() = default;

  // LlamaModelを使用して初期化
  bool Initialize(std::shared_ptr<llama_cpp::LlamaModel> model) {
    m_generator = initializeScoreGenerator(model);
    if (!m_generator) {
      return false;
    }
    return true;
  }

  // テキストの評価を開始
  void Calculate(s3d::String str) {
    if (!m_generator) {
      Console << U"LlamaTextGenerator is not initialized. Call Initialize() "
               U"first.";
      return;
    }

    // 既に計算中の場合はスキップ
    if (is_calculating) {
      Console << U"Score calculation is already in progress.";
      return;
    }

    is_calculating = true;
    m_current_text = str;
    m_calculation_start_time = s3d::Time::GetMillisec();

    // プロンプトを作成（点数のみ算出で最速化）
    llama_cpp::LlmRequest request;
    request.prompt = U"以下のテキストを0-100で評価。数字のみ答えよ:\n" + str;
    request.num_predict_tokens = 5;  // 数字のみなので大幅に削減（50→5）

    // 生成開始
    m_text_buffer.ClearBuffer();
    m_text_buffer.StartGeneration(*m_generator, request);
  }

  // 更新処理（毎フレーム呼び出す）
  void Update() {
    if (is_calculating && m_text_buffer.IsGenerationComplete()) {
      const auto generated_text = m_text_buffer.GetText();
      processGeneratedScore(generated_text);
      is_calculating = false;
    }
  }

  // スコアを取得
  Score GetScore() const {
    std::lock_guard<std::mutex> lock(m_score_lock);
    if (m_score.empty()) {
      return Score{};
    }
    // 最新のスコアを返す
    return m_score.back();
  }

  // すべてのスコアを取得
  std::vector<Score> GetAllScores() const {
    std::lock_guard<std::mutex> lock(m_score_lock);
    return m_score;
  }

  // 計算中かどうかを確認
  bool IsCalculating() const { return is_calculating; }

  private:
  // LlamaTextGeneratorの初期化処理
  static std::shared_ptr<llama_cpp::LlamaTextGenerator> initializeScoreGenerator(
      std::shared_ptr<llama_cpp::LlamaModel> model) {
    // LlamaTextGeneratorを作成
    auto generator = std::make_shared<llama_cpp::LlamaTextGenerator>();

    // コンテキスト設定（最速化）
    llama_cpp::ContextConfig context_config;
    context_config.context_size = 128;
    context_config.batch_size = 128;
    context_config.threads = 8;
    context_config.threads_batch = 8;

    // サンプリング設定（最速化）
    llama_cpp::SamplingConfig sampling_config;
    sampling_config.temperature = 0.1f;  // 数値出力なので確定的に（0.8→0.1）
    sampling_config.top_k = 5;           // 最小限に削減（20→5）
    sampling_config.top_p = 0.5f;        // 大幅削減（0.8→0.5）

    // システムプロンプト（点数のみ）
    s3d::String system_prompt = U"0-100の数字のみ答えるAI";

    // LlamaTextGeneratorを初期化
    auto generator_init_result = generator->InitializeWithModel(
        model, context_config, sampling_config, system_prompt);
    if (!generator_init_result) {
      Console << U"Failed to initialize LlamaTextGenerator: "
            << generator_init_result.error_message;
      return nullptr;
    }

    return generator;
  }

  // 生成されたテキストからスコアを解析
  void processGeneratedScore(const s3d::String& generated_text) {
    Score score;
    score.evaluated_text = m_current_text;
    score.timestamp = s3d::DateTime::Now();
    score.calculation_time_ms = s3d::Time::GetMillisec() - m_calculation_start_time;

    // 生成されたテキストから数字のみを抽出（簡潔化）
    auto trimmed = generated_text.trimmed();

    // 数字のみを抽出（先頭から数字部分を取得）
    s3d::String number_str;
    for (const auto& ch : trimmed) {
      if (ch >= U'0' && ch <= U'9') {
        number_str += ch;
      } else if (!number_str.isEmpty()) {
        // 数字が見つかった後に非数字が来たら終了
        break;
      }
    }

    score.score_value = ParseOr<int>(number_str, 0);

    // スコアを保存
    {
      std::lock_guard<std::mutex> lock(m_score_lock);
      m_score.push_back(score);
    }

    Console << U"Score calculated: " << score.score_value
          << U", Time: " << score.calculation_time_ms << U"ms";
  }

  llama_cpp::LlamaTextBuffer m_text_buffer;
  std::shared_ptr<llama_cpp::LlamaTextGenerator> m_generator;
  std::vector<Score> m_score;
  mutable std::mutex m_score_lock;
  bool is_calculating = false;
  s3d::String m_current_text;  // 現在評価中のテキスト
  double m_calculation_start_time = 0.0;  // 計算開始時刻
};
