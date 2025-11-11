# LlmUtil設計書

## 概要

LLM関連のユーティリティクラス。ゲーム内で使用するLLMモデルの初期化と管理を担当する。

## 目的・スコープ

このクラスは、ゲーム全体で使用されるLLMモデルの初期化処理を目的としており、以下の責務を持つ:

- LLMモデルの読み込みとLlamaModelManagerへの登録
- LLM関連の初期化処理の一元化

ユーティリティクラスとして実装され、インスタンス化を防ぐ。

## ほかのクラスとの関係

- `LlamaModelManager` - LLMモデルの管理を行うマネージャークラス。モデルを登録・取得する
- `JobSearchPhase` - 求職活動フェーズでLLM評価を使用する際に初期化されたモデルを利用
- `SisterMessagePhase` - 妹メッセージフェーズでLLM応答生成を使用する際に初期化されたモデルを利用

## このクラスが継承するクラス

なし

## このクラスのコンストラクタ

インスタンス化を防ぐため、コンストラクタ、コピーコンストラクタ、代入演算子をdelete。

## このクラスのデストラクタ

なし（deleteされたコンストラクタのため）

## このクラスに含まれるデータメンバ

なし

## このクラスに含まれる定数

なし（メソッド内でローカル定数を使用）

## このクラスに含まれるpublicメソッド

- `static void InitializeLLM()` - LLMモデルの初期化処理。kModelLocalPath = U"LlmModel/" + GameConst::kLlmModelId + U".gguf"でパス生成、model_config.model_file_path = FileSystem::CurrentDirectory() + kModelLocalPathでフルパス設定、model_config.num_gpu_layers = 32でGPUレイヤー数設定。model_id = FileSystem::BaseName(kModelLocalPath)で識別子生成。model_manager.InitializeModel(model_id, model_config)で登録、失敗時にConsoleへエラー出力。model_manager.GetModel(model_id)でモデル取得確認、失敗時もConsoleへエラー出力

## このクラスに含まれるprivateメソッド

なし

## このクラスで参照するアセットの情報

- `LlmModel/Qwen3-4B-Instruct-2507-Q8_0.gguf` - 使用するLLMモデルファイル

## このクラスが参照する仕様書の項目

- 求職活動フェーズ
- 妹メッセージフェーズ

## このクラスが使用されるフェーズ

- 求職活動フェーズ
- 妹メッセージフェーズ

## 特記事項・メモ

- ユーティリティクラスとして実装され、インスタンス化を防ぐ（コンストラクタ、コピーコンストラクタ、代入演算子をdelete）
- `InitializeLLM()`は`GameManager`の初期化時に呼び出される
- モデル設定ではGPUレイヤー数を32に設定（model_config.num_gpu_layers = 32）
- モデルファイルパスはGameConst::kLlmModelIdを使用して動的に生成
- モデル識別子はFileSystem::BaseName()で生成され、拡張子を除いたファイル名になる
- 初期化失敗時やモデル取得失敗時はConsole出力でエラーメッセージを表示（例外は使用しない）
