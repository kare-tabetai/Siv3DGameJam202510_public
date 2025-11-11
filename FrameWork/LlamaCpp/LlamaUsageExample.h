﻿  // LlamaUsageExample.h
#pragma once
#include <Siv3D.hpp>

#include "LlamaModelManager.h"
#include "LlamaTextGenerator.h"

  namespace llama_cpp {

  // 使用例：複数のLlamaTextGeneratorで同じモデルを共有
  class LlamaUsageExample {
    public:
    static void RunExample() {
      s3d::Console << U"=== LlamaModelManager 使用例 ===";

      // 1. モデル設定
      ModelConfig model_config;
      model_config.model_file_path = U"LlmModel/gemma-3-4b-it-q4_0.gguf";
      model_config.context_size = 2048;
      model_config.batch_size = 512;
      model_config.num_gpu_layers = 30;
      model_config.threads = 8;
      model_config.threads_batch = 8;

      SamplingConfig sampling_config;
      sampling_config.temperature = 0.7f;
      sampling_config.top_k = 40;
      sampling_config.top_p = 0.9f;
      sampling_config.seed = 1234;

      // 2. モデルマネージャーを使ってモデルを初期化
      const s3d::String model_id = U"gemma-3-4b";
      auto& manager = LlamaModelManager::GetInstance();

      auto init_result = manager.InitializeModel(model_id, model_config);
      if (!init_result) {
        s3d::Console << U"モデル初期化失敗: " << init_result.error_message;
        return;
      }

      s3d::Console << U"モデルが初期化されました: " << model_id;

      // 3. 複数のTextGeneratorを作成（同じモデルを共有）
      LlamaTextGenerator generator1, generator2, generator3;

      // 各ジェネレータを同じモデルで初期化
      auto result1 =
        generator1.Initialize(model_id, model_config, sampling_config);
      auto result2 =
        generator2.Initialize(model_id, model_config, sampling_config);
      auto result3 =
        generator3.Initialize(model_id, model_config, sampling_config);

      if (!result1 || !result2 || !result3) {
        s3d::Console << U"ジェネレータの初期化に失敗しました";
        return;
      }

      s3d::Console << U"3つのTextGeneratorが同じモデルを共有して初期化されました";

      // 4. 異なるプロンプトで並行してテキスト生成
      LlmRequest request1, request2, request3;
      request1.prompt = U"こんにちは、";
      request1.num_predict_tokens = 50;

      request2.prompt = U"今日の天気は";
      request2.num_predict_tokens = 30;

      request3.prompt = U"プログラミングとは";
      request3.num_predict_tokens = 40;

      // 順次実行（実際の並行実行はマルチスレッド環境で）
      s3d::Console << U"=== テキスト生成開始 ===";

      auto gen_result1 = generator1.Generate(request1);
      if (gen_result1.success) {
        s3d::Console << U"Generator1: " << gen_result1.generated_text;
      }

      auto gen_result2 = generator2.Generate(request2);
      if (gen_result2.success) {
        s3d::Console << U"Generator2: " << gen_result2.generated_text;
      }

      auto gen_result3 = generator3.Generate(request3);
      if (gen_result3.success) {
        s3d::Console << U"Generator3: " << gen_result3.generated_text;
      }

      // 5. モデル管理状況の確認
      auto model_ids = manager.GetInitializedModelIds();
      s3d::Console << U"初期化済みモデル数: " << model_ids.size();
      for (const auto& id : model_ids) {
        s3d::Console << U"  - " << id;
      }

      s3d::Console << U"=== 使用例完了 ===";
    }

    // 直接共有モデルを使用する例
    static void RunDirectModelShareExample() {
      s3d::Console << U"=== 直接モデル共有例 ===";

      ModelConfig model_config;
      model_config.model_file_path = U"LlmModel/gemma-3-4b-it-q4_0.gguf";
      model_config.context_size = 1024;
      model_config.batch_size = 256;
      model_config.num_gpu_layers = 20;

      SamplingConfig sampling_config;
      sampling_config.temperature = 0.8f;

      // モデルマネージャーからモデルを取得
      auto& manager = LlamaModelManager::GetInstance();
      const s3d::String model_id = U"gemma-3-4b";

      auto model = manager.GetModel(model_id);
      if (!model) {
        s3d::Console << U"モデルが見つかりません。先に InitializeModel "
                      U"を実行してください。";
        return;
      }

      // 直接モデルを指定してTextGenerator初期化
      LlamaTextGenerator generator;
      auto init_result =
        generator.InitializeWithModel(model, model_config, sampling_config);

      if (init_result) {
        s3d::Console << U"共有モデルを使ってTextGeneratorを初期化しました";
      } else {
        s3d::Console << U"初期化失敗: " << init_result.error_message;
      }
    }
  };

}  // namespace llama_cpp
