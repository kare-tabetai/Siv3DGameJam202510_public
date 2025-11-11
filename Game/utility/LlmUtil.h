#pragma once
#include <Siv3D.hpp>

#include "Game/utility/DebugUtil.h"
#include "Game/utility/GameConst.h"
#include "LlamaCpp/LlamaModelManager.h"

// LLM関連のユーティリティクラス
class LlmUtil {
public:
	// LLMモデルの初期化を行う
	static void InitializeLLM() {
		// モデル設定（モデルファイル読み込み用）
		llama_cpp::ModelConfig model_config;


		const FilePath kModelLocalPath = U"LlmModel/" + GameConst::kLlmModelId + U".gguf";
		model_config.model_file_path = FileSystem::CurrentDirectory() + kModelLocalPath;
		model_config.num_gpu_layers = 32;

		const s3d::String model_id = FileSystem::BaseName(kModelLocalPath);

		// LlamaModelManagerにモデルを登録
		auto& model_manager = llama_cpp::LlamaModelManager::GetInstance();
		auto model_init_result = model_manager.InitializeModel(model_id, model_config);
		if (!model_init_result) {
			DebugUtil::Console << U"Failed to initialize model in LlamaModelManager: " << model_init_result.error_message;
			return;
		}

		// 登録したモデルを取得
		auto shared_model = model_manager.GetModel(model_id);
		if (!shared_model) {
			DebugUtil::Console << U"Failed to get model from LlamaModelManager: " << model_id;
			return;
		}
	}

private:
	// インスタンス化を防ぐ
	LlmUtil() = delete;
	LlmUtil(const LlmUtil&) = delete;
	LlmUtil& operator=(const LlmUtil&) = delete;
};
