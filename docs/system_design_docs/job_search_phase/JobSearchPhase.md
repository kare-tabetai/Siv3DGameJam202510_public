# JobSearchPhase設計書

## 概要

求職活動フェーズ全体を管理するクラス。パスワード入力演出、サーバーローディング、オートコンプリート、履歴書入力、LLMによる評価、結果表示までの一連の流れを制御する。6つの状態(PasswordInput/AutoCompleting/ShowingInstruction/ResumeInput/Evaluating/ShowingResult)を持つ。

## 目的・スコープ

プレイヤーが履歴書を作成し、AIによる採用判定を受けるまでの求職活動フェーズの処理を担当する。パスワード入力演出→サーバーローディング→オートコンプリート→(初回のみ)操作説明→履歴書UI→ローディング→結果表示という流れを管理し、LLMによる評価結果に基づいてゲームの次のフェーズへの遷移を決定する。

## ほかのクラスとの関係

- `PhaseManager` - フェーズ遷移の指示を行う（iPhase経由で呼び出される）
- `GameCommonData` - 精神力と残り日数の情報を参照・更新する
- `PasswordInputUI` - パスワード入力演出を制御する
- `ResumeUI` - 履歴書UIの表示と入力を制御する
- `ServerLoadingUI` - サーバーローディング演出を制御する
- `LoadingUI` - LLM評価待ちのローディング画面を制御する
- `RejectionListUI` - 不採用結果の表示を制御する
- `MessageWindowUI` - 初回プレイ時の操作説明を表示する
- `LlamaTextGenerator` - 履歴書の自己PR/志望動機をLLMで評価する
- `LlamaTextBuffer` - LLM生成テキストのバッファ管理
- `LlamaModelManager` - LLMモデルの取得
- `SoundManager` - 効果音とBGMの再生を指示する
- `MouseEffectManager` - マウスエフェクトの設定
- `BackGroundManager` - 背景画像の設定

## このクラスが継承するクラス

- `iPhase` - Siv3Dのシーン管理クラス

## このクラスのコンストラクタ

- `JobSearchPhase()` - シーン初期化データを受け取り、各UIコンポーネントとLLMジェネレーターを初期化する

## このクラスのデストラクタ

デフォルトデストラクタを使用

## このクラスに含まれるデータメンバ

- `PasswordInputUI passwordInputUI_` - パスワード入力演出UIのインスタンス
- `ResumeUI resumeUI_` - 履歴書UIのインスタンス
- `LoadingUI loadingUI_` - ローディングUIのインスタンス
- `RejectionListUI rejectionListUI_` - 不採用リストUIのインスタンス
- `ServerLoadingUI serverLoadingUI_` - サーバーローディングUIのインスタンス（追加）
- `std::unique_ptr<llama_cpp::LlamaTextGenerator> llmGenerator_` - LLMテキスト生成器
- `llama_cpp::LlamaTextBuffer llmTextBuffer_` - LLMテキストバッファ
- `String selfPRText_` - プレイヤーが入力した自己PR/志望動機のテキスト
- `int32 evaluationScore_` - LLMによる評価スコア(0-100、初期値0)
- `PhaseState currentState_` - 現在のフェーズ内の状態(PasswordInput/AutoCompleting/ShowingInstruction/ResumeInput/Evaluating/ShowingResult、初期値PasswordInput)

## このクラスに含まれる定数

### 状態定義

- `enum class PhaseState` - フェーズ内の状態を表す列挙型
  - `PasswordInput` - パスワード入力中
  - `AutoCompleting` - オートコンプリート待ち
  - `ShowingInstruction` - 初回操作説明表示中
  - `ResumeInput` - 履歴書入力中
  - `Evaluating` - LLM評価中
  - `ShowingResult` - 結果表示中

### スコア関連

- `static constexpr int32 kDay1PassingScore = 95` - 初日の合格ライン(95点以上)
- `static constexpr int32 kHalfwayPassingScore = 80` - 残り日数半分以下での合格ライン(80点以上)
- `static constexpr int32 kFinalDayPassingScore = 90` - 最終日の合格ライン(90点以上)
- `static constexpr double kMentalDamageCoefficient = 0.1` - 精神力減少係数(10 - score/10)

### UI関連

- `static constexpr StringView kServerLoadingMessage` - サーバー通信中メッセージ（"国民統合情報サーバーと通信中...\n基本情報・職歴情報・資格情報を取得中..."）

## このクラスに含まれるpublicメソッド

- `static std::unique_ptr<llama_cpp::LlamaTextGenerator> CreateLlamaTextGenerator()` - LLMテキストジェネレーターを作成するstaticメソッド。LlamaModelManagerからモデルを取得し、コンテキスト設定(context_size=256, batch_size=256, threads=8, threads_batch=8)、サンプリング設定(temperature=0.1, top_k=5, top_p=0.5)、システムプロンプトを設定して初期化する
- `JobSearchPhase()` - コンストラクタ。LLMジェネレーターを初期化し、各UIの初期化、BGM再生、背景設定を行う
- `~JobSearchPhase()` - デフォルトデストラクタ
- `void update() override` - 毎フレーム呼ばれる更新処理。現在の状態(PhaseState)に応じて各UIの更新とLLM評価の進行を管理する
- `void draw() const override` - 毎フレーム呼ばれる描画処理。現在の状態(PhaseState)に応じて各UIを描画する

## このクラスに含まれるprivateメソッド

- `void UpdatePasswordInput()` - パスワード入力演出の更新処理。パスワード入力が完了したらAutoCompleting状態に遷移し、ResumeUIのオートコンプリートを開始、ServerLoadingUIを表示
- `void UpdateAutoCompleting()` - オートコンプリート更新処理。ResumeUIのオートコンプリートが完了したら、初回プレイ時は操作説明を表示(ShowingInstruction)、それ以外は履歴書入力(ResumeInput)に遷移
- `void UpdateShowingInstruction()` - 操作説明表示中の更新処理。MessageWindowUIが完了したら履歴書入力(ResumeInput)に遷移
- `void ShowInstruction()` - 初回プレイ時の操作説明を表示する（MessageWindowUIを使用）
- `void UpdateResumeInput()` - 履歴書入力の更新処理。入力が確定されたらLLM評価(Evaluating)に遷移
- `void UpdateEvaluation()` - LLM評価の更新処理。生成完了を待ち、生成テキストから数字のみを抽出してスコアを算出。採用/不採用に応じて次の処理へ遷移
- `void UpdateResultDisplay()` - 結果表示の更新処理。RejectionListUIが閉じられたら次のフェーズへ遷移
- `void StartLLMEvaluation(const String& selfPR)` - LLMによる評価を開始する。LlmRequestを作成(num_predict_tokens=25)し、LlamaTextBufferで生成開始
- `[[nodiscard]] int32 CalculatePassingScore() const` - 現在の日数に応じた合格ラインを計算する（初日: 95点、最終日: 90点、残り日数半分以下: 80点、それ以外: 95点）
- `[[nodiscard]] bool IsPassingScore(int32 score) const` - スコアが合格ラインを超えているかを判定する
- `void ApplyMentalDamage(int32 score)` - 不採用時の精神力減少を適用する（damage = 10 - score * 0.1）
- `void TransitionToNextPhase()` - 次のフェーズへの遷移を決定・実行する（精神力0→GameOver、最終日不採用→BadEnd、採用→GameClear、それ以外→SisterMessage）

## このクラスで参照するアセットの情報

- BGM: `Asset/bgm_room.mp3` - 室内環境音
- SE: `Asset/keyboard/タイピング-メカニカル単1.mp3` - キーボード打鍵音1
- SE: `Asset/keyboard/タイピング-メカニカル単2.mp3` - キーボード打鍵音2
- SE: `Asset/keyboard/タイピング-メカニカル単3.mp3` - キーボード打鍵音3
- SE: `Asset/PC-Mouse05-1.mp3` - マウスクリック音
- 背景画像: `Asset/background.png` - UI背景画像
- LLMモデル: `LlmModel/Qwen3-4B-Instruct-2507-Q8_0.gguf` - ローカルLLMモデル

## このクラスが参照する仕様書の項目

- 求職活動フェーズ
- ローカルLLMによる評価
- 精神力
- フェーズ遷移図

## このクラスが使用されるフェーズ

- 求職活動フェーズ

## 特記事項・メモ

- LLM評価は非同期で行い、評価中はLoadingUIを表示してプレイヤーの待ち時間を軽減する
- 評価プロンプトは"以下の自己PR/志望動機を0-100で評価してください。評価は数字のみで答えてください。:"を使用
- 最終日に不採用の場合、精神力が残っていてもバッドエンドフェーズへ遷移する
- 不採用時の精神力減少量は`10 - (score * kMentalDamageCoefficient)`で計算する
- PhaseState enumは以下の状態を持つ: PasswordInput, ResumeInput, Evaluating, ShowingResult

追加実装ノート（ヘッダ実装に合わせた詳細）:

- `CreateLlamaTextGenerator()` の実装がフェーズ内に用意されており、LLMモデルは `LlamaModelManager::GetInstance().GetModel(GameConst::kLlmModelId)` から取得する設計になっている。
- 初期化時に `llama_cpp::ContextConfig` と `llama_cpp::SamplingConfig` を設定している（実装例: context_size=256, batch_size=256, threads=8, threads_batch=8 / sampling: temperature=0.1, top_k=5, top_p=0.5）。これらは性能・レイテンシ最適化向けの値で、実行環境に応じて調整可能。
- システムプロンプトは LLM に対して「採用担当者として0-100のスコアのみを返す」ことを明示する短い文を与えている（ヘッダ実装に文字列リテラルとして含まれる）。
- `StartLLMEvaluation(const String& selfPR)` は `llama_cpp::LlmRequest` を利用して生成を開始する（実装では `num_predict_tokens = 25` を指定）。生成は `llmTextBuffer_.StartGeneration(*llmGenerator_, request)` で非同期に開始される。
- 生成結果は `llmTextBuffer_.IsGenerationComplete()` を待ち、取得したテキストから数字文字のみを抽出してスコアを算出する実装になっている（実装例では数字以外を取り除いてから ParseOr<int32>(..., 0) で整数へ変換）。

これらの実装詳細は Llama 関連のフレームワーク API（`FrameWork/LlamaCpp/*`）に依存するため、API 仕様やモデルファイルの有無によって動作が変わる点に注意する。

**実装参照:** LLM ジェネレータ実装は `FrameWork/LlamaCpp/LlamaTextGenerator.h` にある。実装を参照して初期化やコール方法を合わせてください。
