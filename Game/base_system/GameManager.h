// GameManager.h
#pragma once
#include <Siv3D.hpp>
#include <memory>

#include "BackGroundManager.h"
#include "BlackOutUI.h"
#include "CommonUI.h"
#include "Game/utility/FontManager.h"
#include "Game/utility/GameConst.h"
#include "Game/utility/MouseEffectManager.h"
#include "Game/utility/PhaseType.h"
#include "Game/utility/SoundManager.h"
#include "GameCommonData.h"
#include "MessageWindowUI.h"
#include "PhaseManager.h"
#include "SisterMessageUIManager.h"
#if defined(_DEBUG) || defined(DEBUG)
#include "DebugSystem.h"
#endif

// 各Phaseクラスのinclude
#include "Game/bad_end_phase/BadEndPhase.h"
#include "Game/game_clear_phase/GameClearPhase.h"
#include "Game/game_over_phase/GameOverPhase.h"
#include "Game/introduction_phase/IntroductionPhase.h"
#include "Game/job_search_phase/JobSearchPhase.h"
#include "Game/night_dream_phase/NightDreamPhase.h"
#include "Game/sister_message_phase/SisterMessagePhase.h"
#include "Game/sunrise_phase/SunrisePhase.h"
#include "Game/sunset_phase/SunsetPhase.h"
#include "Game/work_phase/WorkPhase.h"

// ゲーム全体を管理し、ゲームプレイ中は常駐するクラス
class GameManager {
  public:
  // コンストラクタ
  static void Initialize() {
    // フォントとサウンドシステムを初期化
    FontManager::Initialize();
    SoundManager::Initialize();

    // 背景マネージャーを初期化
    BackGroundManager::Initialize();

    // モノステートクラスを初期化
    GameCommonData::Initialize();
    CommonUI::Initialize();
    MessageWindowUI::Initialize();
    BlackOutUI::Initialize();

    // SisterMessageUIを初期化
    SisterMessageUIManager::Initialize();

    // PhaseManagerを初期化
    PhaseManager::Initialize();

    // MouseEffectManagerを初期化
    MouseEffectManager::Initialize();

    // 各PhaseTypeに対応する生成関数を登録
    RegisterPhaseFactories();

    // 初期フェーズを設定（IntroductionPhase）
    PhaseManager::ChangePhase(GameConst::kInitialPhase);
  }

  private:
  // 各PhaseTypeに対応する生成関数を登録
  static void RegisterPhaseFactories() {
    PhaseManager::RegisterPhaseFactory(PhaseType::Introduction, []() {
      return std::make_shared<IntroductionPhase>();
    });
    PhaseManager::RegisterPhaseFactory(PhaseType::Sunrise, []() {
      return std::make_shared<SunrisePhase>();
    });
    PhaseManager::RegisterPhaseFactory(PhaseType::JobSearch, []() {
      return std::make_shared<JobSearchPhase>();
    });
    PhaseManager::RegisterPhaseFactory(PhaseType::SisterMessage, []() {
      return std::make_shared<SisterMessagePhase>();
    });
    PhaseManager::RegisterPhaseFactory(PhaseType::Work, []() {
      return std::make_shared<WorkPhase>();
    });
    PhaseManager::RegisterPhaseFactory(PhaseType::Sunset, []() {
      return std::make_shared<SunsetPhase>();
    });
    PhaseManager::RegisterPhaseFactory(PhaseType::NightDream, []() {
      return std::make_shared<NightDreamPhase>();
    });
    PhaseManager::RegisterPhaseFactory(PhaseType::GameOver, []() {
      return std::make_shared<GameOverPhase>();
    });
    PhaseManager::RegisterPhaseFactory(PhaseType::BadEnd, []() {
      return std::make_shared<BadEndPhase>();
    });
    PhaseManager::RegisterPhaseFactory(PhaseType::GameClear, []() {
      return std::make_shared<GameClearPhase>();
    });
  }

  public:
  // デストラクタ
  ~GameManager() = default;

  // 更新処理
  static void Update() {

    MouseEffectManager::Update();

    // サウンドマネージャーの更新（クロスフェード処理のため必須）
    SoundManager::Update(Scene::DeltaTime());

    // 共通UIの更新
    BlackOutUI::Update();
    MessageWindowUI::Update();

    // SisterMessageUIの更新
    SisterMessageUIManager::Update();

    // Phaseの更新
    PhaseManager::Update();

    // デバッグ表示を更新（デバッグビルドのみ）
#if defined(_DEBUG) || defined(DEBUG)
    DebugSystem::Update();
#endif

  }

  // 描画処理
  static void Draw() {
    // 背景を描画
    BackGroundManager::Draw();

    // SisterMessageUIを描画
    SisterMessageUIManager::Draw();

    // Phaseの描画
    PhaseManager::Draw();

    // 共通UIを描画
    CommonUI::Draw();
    MessageWindowUI::Draw();

    // 暗転UIを描画
    BlackOutUI::Draw();

    // PhaseのpostDraw処理を呼び出し（暗転の手前の描画があればここで実行）
    PhaseManager::PostDraw();

    MouseEffectManager::Draw();

    // デバッグ表示を最前面に描画（デバッグビルドのみ）
#if defined(_DEBUG) || defined(DEBUG)
    DebugSystem::Draw();
#endif

  }

};
