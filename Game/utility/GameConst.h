#pragma once
#include <Siv3D.hpp>

#include "Game/utility/PhaseType.h"

// GameConst.h
// ゲーム全体で使う定数を集約する静的クラス
// - ヘッダオンリーで利用する想定
// - ここに定数を追加してプロジェクト内から参照してください

// GameConst はインスタンス化させない静的クラス風の構造体
struct GameConst {
  // コンストラクタ/デストラクタを隠蔽してインスタンス化を禁止
  GameConst() = delete;
  ~GameConst() = delete;

  // 画面サイズ（ピクセル）
  static constexpr auto kWindowSize = Point{1280, 720} ;

  // ウィンドウタイトル
  static constexpr StringView kWindowTitle = U"2031_06_20-23";

  // ゲーム開始時の初期フェーズ
  static constexpr PhaseType kInitialPhase = PhaseType::Introduction;

  static constexpr double kCommonUIHeight = kWindowSize.y / 13;  // 共通UIの高さ

  // プレイエリアの矩形
  static constexpr RectF kPlayAreaRect = RectF{0, kCommonUIHeight, kWindowSize.x, kWindowSize.y - kCommonUIHeight};

   // プレイエリアの中心位置
  static constexpr Vec2 kPlayAreaCenter = kPlayAreaRect.center();

  // プレイエリアのサイズ
  static constexpr Vec2 kPlayAreaSize = kPlayAreaRect.size;

  // LLMモデルID
  static constexpr StringView kLlmModelId = U"Qwen3-4B-Instruct-2507-Q8_0";

  // アルバイトフェーズのエリアの幅
  static constexpr double kWorkAreaWidth = 990.0;

};  // struct GameConst
