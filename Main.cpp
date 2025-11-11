#define _CRT_SECURE_NO_WARNINGS
#include <Siv3D.hpp>

#include "Game/base_system/GameManager.h"
#include "Game/utility/GameConst.h"
#include "Game/utility/LlmUtil.h"
#include "Helper/LicenseHelper.h"
#include "External/60FPSwithAutoFrameSkip.h"

void Main() {
  LicenseHelper::AddLicenses();

  Graphics::SetVSyncEnabled(true);

  Window::Resize(GameConst::kWindowSize);
  Window::SetTitle(GameConst::kWindowTitle);

  // LLMモデルの初期化
  LlmUtil::InitializeLLM();

  // GameManagerの初期化
  GameManager::Initialize();

  while (System::Update()) {
    // 背景をクリア
    Scene::SetBackground(ColorF(0.8, 0.9, 1.0));

    GameManager::Update();
    GameManager::Draw();
  }
}
