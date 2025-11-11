// MouseEffectManager.h
// マウスクリック時の視覚的・聴覚的フィードバックを管理するクラス

#pragma once
#include <Siv3D.hpp>

#include "SoundManager.h"
#include "TweenUtil.h"

// マウスクリック時の視覚エフェクト（広がる輪）と音を管理
// モノステートパターンで実装され、どこからでもアクセス可能
class MouseEffectManager {
  public:
  // コンストラクタ/デストラクタは削除（モノステート）
  MouseEffectManager() = delete;
  ~MouseEffectManager() = delete;

  // クラスの初期化。ゲーム開始時に1度だけ呼び出す
  static void Initialize() {
    if (isInitialized_) {
      return;
    }

    activeEffects_.clear();
    isInitialized_ = true;
  }

  // 毎フレーム呼び出し、アクティブなエフェクトを更新する
  // MouseL.down()を使用してクリック判定を行い、クリック時にエフェクトを追加してSEを再生する
  // 持続時間を超えたエフェクトも削除
  static void Update() {
    if (!isInitialized_) {
      return;
    }

    // マウス左クリック判定
    if (MouseL.down()) {
      AddEffect(Cursor::Pos());
      PlayClickSound();
    }

    // 持続時間を超えたエフェクトを削除
    activeEffects_.remove_if([](const ClickEffect& effect) {
      return effect.timer.sF() >= effect.duration;
    });
  }

  // 毎フレーム呼び出し、すべてのアクティブなエフェクトを描画する
  static void Draw() {
    if (!isInitialized_) {
      return;
    }

    for (const auto& effect : activeEffects_) {
      DrawEffect(effect);
    }
  }

  // すべてのアクティブなエフェクトをクリアする
  // フェーズ遷移時などに使用
  static void Clear() {
    if (!isInitialized_) {
      return;
    }

    activeEffects_.clear();
  }

  static void SetActiveSound(bool active) {
    isSoundActive_ = active;
  }

  // 定数
  static constexpr double kEffectDuration = 0.5;  // エフェクトの持続時間（秒）
  static constexpr double kMaxRadius = 40.0;      // 輪の最大半径（ピクセル）
  static constexpr double kLineThickness = 3.0;   // 輪の線の太さ（ピクセル）
  static constexpr ColorF kEffectColor{0.8, 0.8, 1.0,
                                       1.0};  // エフェクトの色（明るい青白）

  private:
  // 内部構造体: 個別のクリックエフェクト
  struct ClickEffect {
    Vec2 position;     // クリック位置の座標
    Stopwatch timer;   // エフェクト開始からの経過時間を計測
    double duration;   // エフェクトの持続時間（秒）
    ColorF color;      // エフェクトの色
    double maxRadius;  // 輪の最大半径（ピクセル）
  };

  // 新しいエフェクトをactiveEffects_に追加する内部処理
  static void AddEffect(const Vec2& position) {
    ClickEffect effect;
    effect.position = position;
    effect.timer.start();
    effect.duration = kEffectDuration;
    effect.color = kEffectColor;
    effect.maxRadius = kMaxRadius;

    activeEffects_.push_back(effect);
  }

  // 個別のエフェクトを描画する内部処理
  // 経過時間に応じて半径とアルファ値を計算
  static void DrawEffect(const ClickEffect& effect) {
    const double t = effect.timer.sF() / effect.duration;
    const double progress = TweenUtil::EaseOut(t);

    // 半径は時間とともに広がる
    const double radius = progress * effect.maxRadius;

    // アルファ値は時間とともに減少
    const double alpha = (1.0 - progress) * effect.color.a;

    ColorF drawColor = effect.color;
    drawColor.a = alpha;

    Circle{effect.position, radius}.drawFrame(kLineThickness, drawColor);
  }

  // クリック音をSoundManager経由で再生する内部処理
  static void PlayClickSound() {
    if (!isSoundActive_) {
      return;
    }
    SoundManager::PlaySE(U"se_click");
   }

  // データメンバ
  static inline Array<ClickEffect>
    activeEffects_;                           // 現在再生中のエフェクトの配列
  static inline bool isInitialized_ = false;  // 初期化済みかどうかのフラグ

  static inline bool isSoundActive_ = true;  // クリック音の有効/無効フラグ
};
