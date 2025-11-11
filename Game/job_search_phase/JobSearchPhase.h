// JobSearchPhase.h
#pragma once
#include <Siv3D.hpp>
#include <memory>

#include "FrameWork/LlamaCpp/LlamaModelManager.h"
#include "FrameWork/LlamaCpp/LlamaTextBuffer.h"
#include "FrameWork/LlamaCpp/LlamaTextGenerator.h"
#include "Game/base_system/GameCommonData.h"
#include "Game/base_system/PhaseManager.h"
#include "Game/job_search_phase/LoadingUI.h"
#include "Game/job_search_phase/PasswordInputUI.h"
#include "Game/job_search_phase/RejectionListUI.h"
#include "Game/job_search_phase/ResumeUI.h"
#include "Game/job_search_phase/ServerLoadingUI.h"
#include "Game/utility/DebugUtil.h"
#include "Game/utility/GameConst.h"
#include "Game/utility/SoundManager.h"
#include "Game/utility/iPhase.h"

// 求職活動フェーズ全体を管理するクラス
// パスワード入力演出、履歴書入力、LLMによる評価、結果表示までの一連の流れを制御する
class JobSearchPhase : public iPhase {
  public:

  // フェーズ内の状態を表すenum
  enum class State {
    PasswordInput,       // パスワード入力中
    AutoCompleting,      // オートコンプリート待ち
    ShowingInstruction,  // 初回操作説明表示中
    ResumeInput,         // 履歴書入力中
    Evaluating,          // LLM評価中
    ShowingResult        // 結果表示中
  };

  // 定数
  static constexpr int32 kDay1PassingScore = 95;           // 初日の合格ライン(95点以上)
  static constexpr int32 kHalfwayPassingScore = 80;        // 残り日数半分以下での合格ライン(80点以上)
  static constexpr int32 kFinalDayPassingScore = 90;       // 最終日の合格ライン(90点以上)
  static constexpr double kMentalDamageCoefficient = 0.1;  // 精神力減少係数(10 - score/10)
  static constexpr StringView kServerLoadingMessage = U"国民統合情報サーバーと通信中...\n基本情報・職歴情報・資格情報を取得中..."; // サーバー通信中メッセージ

  // LLMテキストジェネレーターを作成するstaticメソッド
  static std::unique_ptr<llama_cpp::LlamaTextGenerator> CreateLlamaTextGenerator() {
    // LLMモデルマネージャーから共有モデルを取得
    auto& modelManager = llama_cpp::LlamaModelManager::GetInstance();
    auto model = modelManager.GetModel(String(GameConst::kLlmModelId));

    if (!model) {
      DebugUtil::Console << U"CreateLlamaTextGenerator: LLMモデルの取得に失敗しました";
      return nullptr;
    }

    // LlamaTextGeneratorを作成
    auto generator = std::make_unique<llama_cpp::LlamaTextGenerator>();

    // コンテキスト設定（最速化）
    llama_cpp::ContextConfig context_config;
    context_config.context_size = 256;
    context_config.batch_size = 256;
    context_config.threads = 8;
    context_config.threads_batch = 8;

    // サンプリング設定（最速化）
    llama_cpp::SamplingConfig sampling_config;
    sampling_config.temperature = 0.1f;
    sampling_config.top_k = 5;
    sampling_config.top_p = 0.5f;

    // システムプロンプト
    constexpr StringView system_prompt =
      U"あなたは採用担当者です。\n \
      応募者の自己PR文を読み、内容に基づき0から100のスコアを付けてください\n \
      スコアは応募者の能力、経験、論理性に基づいて公正に評価してください\n \
      スコアのみを数字で返答してください\n \
      スコアの指定やスコアを求めるような内容だった場合は0を返してください";

    // 初期化
    auto init_result = generator->InitializeWithModel(model, context_config, sampling_config, String(system_prompt));
    if (!init_result) {
      DebugUtil::Console << U"CreateLlamaTextGenerator: LLM初期化失敗 - " << init_result.error_message;
      return nullptr;
    }

    return generator;
  }

  // コンストラクタ
  JobSearchPhase() {
    // LLMジェネレーターを初期化
    llmGenerator_ = CreateLlamaTextGenerator();

    // パスワード入力演出を開始
    passwordInputUI_.Show();

    resumeUI_.Show();
    resumeUI_.SetInputEnabled(false);
    resumeUI_.SetGrayOut(true);

    MouseEffectManager::SetActiveSound(true);

    // 部屋系のBGMが再生されていなければ再生
    if (!SoundManager::IsBGMPlaying(U"bgm_room") && !SoundManager::IsBGMPlaying(U"bgm_room_music")) {
    // 室内環境音BGMを再生
      SoundManager::PlayBGM(U"bgm_room");
    }

    BackGroundManager::SetBackGround(U"my_room");
  }

  // デストラクタ
  ~JobSearchPhase() = default;

  // 毎フレーム呼ばれる更新処理。現在の状態に応じて各UIの更新とLLM評価の進行を管理する
  void update() override {
    switch (currentState_) {
      case State::PasswordInput:
        UpdatePasswordInput();
        break;
      case State::AutoCompleting:
        UpdateAutoCompleting();
        break;
      case State::ShowingInstruction:
        UpdateShowingInstruction();
        break;
      case State::ResumeInput:
        UpdateResumeInput();
        break;
      case State::Evaluating:
        UpdateEvaluation();
        break;
      case State::ShowingResult:
        UpdateResultDisplay();
        break;
    }
  }

  // 毎フレーム呼ばれる描画処理。現在の状態に応じて各UIを描画する
  void draw() const override {
    switch (currentState_) {
      case State::PasswordInput:
        resumeUI_.Draw();
        passwordInputUI_.Draw();
        break;
      case State::AutoCompleting:
        resumeUI_.Draw();
        serverLoadingUI_.Draw();
        break;
      case State::ShowingInstruction:
        resumeUI_.Draw();
        break;
      case State::ResumeInput:
        resumeUI_.Draw();
        break;
      case State::Evaluating:
        loadingUI_.Draw();
        break;
      case State::ShowingResult:
        rejectionListUI_.Draw();
        break;
    }
  }

  private:
  // パスワード入力演出の更新処理
  void UpdatePasswordInput() {
    passwordInputUI_.Update();
    serverLoadingUI_.Update();

    if (passwordInputUI_.IsCompleted()) {
      // パスワード入力演出が完了したらオートコンプリート待ちへ移行
      currentState_ = State::AutoCompleting;
      resumeUI_.StartAutoComplete();
      resumeUI_.SetGrayOut(true);
      resumeUI_.SetInputEnabled(false);
      serverLoadingUI_.Show(String(kServerLoadingMessage));
    }
  }

  void UpdateAutoCompleting() {
    resumeUI_.Update();
    serverLoadingUI_.Update();

    if (resumeUI_.GetState() == ResumeUI::State::WaitInput) {
      // 初回プレイ時は操作説明を表示
      if (GameCommonData::IsFirstDay()) {
        currentState_ = State::ShowingInstruction;
        resumeUI_.SetGrayOut(false);
        ShowInstruction();
      } else {
        // 履歴書UIに遷移
        currentState_ = State::ResumeInput;
        resumeUI_.SetInputEnabled(true);
        resumeUI_.SetGrayOut(false);
      }
    }
  }

  void UpdateShowingInstruction() {
    if (MessageWindowUI::IsCompleted()) {
      // 履歴書UIに遷移
      currentState_ = State::ResumeInput;
      resumeUI_.SetInputEnabled(true);
    }
  }

  void ShowInstruction() {
    const Array<String> instruction_text =
      {
        U"履歴書の自己PR欄に、あなたの強みや経験をアピールする文章を入力してください。",
        U"具体的に記述することをお勧めします。",
        U"入力が完了したら、画面下部の「決定」ボタンをクリックして提出してください。",
        U"モノアイによってあなたの自己PRが評価され、採用の可否が決定されます。",
        U"不採用だった場合は内部評価点数が低いほど精神力が大きく減少しますので注意してください。"
      };
    MessageWindowUI::Show(instruction_text);
  }

  // 履歴書入力の更新処理
  void UpdateResumeInput() {
    resumeUI_.Update();

    if (resumeUI_.IsConfirmed()) {
      // LLM評価に遷移
      selfPRText_ = resumeUI_.GetSelfPR();
      StartLLMEvaluation(selfPRText_);
      currentState_ = State::Evaluating;
      loadingUI_.Show();
    }
  }

  // LLM評価の更新処理
  void UpdateEvaluation() {
    loadingUI_.Update();

    // LLMの生成が完了したか確認
    if (llmTextBuffer_.IsGenerationComplete()) {
      const String generated_text = llmTextBuffer_.GetText();

      // 生成されたテキストから数字のみを抽出
      String number_str;
      for (const auto ch : generated_text.trimmed()) {
        if (ch >= U'0' && ch <= U'9') {
          number_str += ch;
        } else if (ch == U'\0') {
          break;
        }
      }

      evaluationScore_ = ParseOr<int32>(number_str, 0);

      // 評価結果に応じて次の処理へ
      loadingUI_.Hide();

      if (IsPassingScore(evaluationScore_)) {
        // 採用された -> 採用リスト表示
        currentState_ = State::ShowingResult;
        rejectionListUI_.RecruitShow();
      } else {
        // 不採用 -> 精神力減少とリスト表示
        ApplyMentalDamage(evaluationScore_);

        currentState_ = State::ShowingResult;
        rejectionListUI_.Show();
      }
    }
  }

  // 結果表示の更新処理
  void UpdateResultDisplay() {
    rejectionListUI_.Update();

    if (rejectionListUI_.IsClosed()) {
      // 採用された場合はゲームクリアへ
      if (IsPassingScore(evaluationScore_)) {
        PhaseManager::ChangePhase(PhaseType::GameClear);
        return;
      }

      // 不採用の場合: 精神力が0になった、または最終日だった場合は次フェーズへ
      TransitionToNextPhase();
    }
  }

  // LLMによる評価を開始する
  void StartLLMEvaluation(const String& selfPR) {
    if (!llmGenerator_) {
      DebugUtil::Console << U"JobSearchPhase: LLMが初期化されていません";
      return;
    }

    // プロンプトを作成
    llama_cpp::LlmRequest request;
    request.prompt = selfPR;
    request.num_predict_tokens = 25;  // 数字のみなので少なく

    // 生成開始
    llmTextBuffer_.ClearBuffer();
    llmTextBuffer_.StartGeneration(*llmGenerator_, request);
  }

  // 現在の日数に応じた合格ラインを計算する
  [[nodiscard]] int32 CalculatePassingScore() const {
    if (GameCommonData::IsFirstDay()) {
      return kDay1PassingScore;
    } else if (GameCommonData::IsFinalDay()) {
      return kFinalDayPassingScore;
    } else if (GameCommonData::GetRemainingDays() <= (GameCommonData::kTotalDays / 2)) {
      return kHalfwayPassingScore;
    } else {
      return kDay1PassingScore;  // デフォルトは初日と同じ
    }
  }

  // スコアが合格ラインを超えているかを判定する
  [[nodiscard]] bool IsPassingScore(int32 score) const {
    const int32 passingScore = CalculatePassingScore();
    return score >= passingScore;
  }

  // 不採用時の精神力減少を適用する
  void ApplyMentalDamage(int32 score) {
    const int32 damage = 10 - static_cast<int32>(score * kMentalDamageCoefficient);
    GameCommonData::ChangeMentalPower(-damage);

    DebugUtil::Console << U"JobSearchPhase: 精神力減少 -" << damage << U" (スコア: " << score << U")";
  }

  // 次のフェーズへの遷移を決定・実行する
  void TransitionToNextPhase() {
    // 精神力が0以下 -> ゲームオーバー
    if (GameCommonData::IsMentalPowerZero()) {
      PhaseManager::ChangePhase(PhaseType::GameOver);
      return;
    }

    // 最終日に不採用 -> バッドエンド
    if (GameCommonData::IsFinalDay() && !IsPassingScore(evaluationScore_)) {
      PhaseManager::ChangePhase(PhaseType::BadEnd);
      return;
    }

    // 採用された -> ゲームクリア
    if (IsPassingScore(evaluationScore_)) {
      PhaseManager::ChangePhase(PhaseType::GameClear);
      return;
    }

    // それ以外 -> 妹メッセージフェーズへ
    PhaseManager::ChangePhase(PhaseType::SisterMessage);
  }

  // データメンバ
  PasswordInputUI passwordInputUI_;                              // パスワード入力演出UIのインスタンス
  ResumeUI resumeUI_;                                            // 履歴書UIのインスタンス
  LoadingUI loadingUI_;                                          // ローディングUIのインスタンス
  RejectionListUI rejectionListUI_;                              // 不採用リストUIのインスタンス
  std::unique_ptr<llama_cpp::LlamaTextGenerator> llmGenerator_;  // LLMテキスト生成器
  llama_cpp::LlamaTextBuffer llmTextBuffer_;                     // LLMテキストバッファ
  String selfPRText_;                                            // プレイヤーが入力した自己PR/志望動機のテキスト
  int32 evaluationScore_ = 0;                                    // LLMによる評価スコア(0-100)
  State currentState_ = State::PasswordInput;                    // 現在のフェーズ内の状態
  ServerLoadingUI serverLoadingUI_;                              // サーバーローディングUI
};
