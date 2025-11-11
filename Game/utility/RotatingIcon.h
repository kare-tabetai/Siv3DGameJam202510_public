// RotatingIcon.h
#pragma once
#include <Siv3D.hpp>

#include "Game/utility/UiConst.h"

// 回転するローディングアイコンを担当するヘッダオンリーのユーティリティクラス
// - テクスチャの読み込み、回転・スケール変調、描画までをカプセル化する
class RotatingIcon {
  public:
  static inline const String kLoadingIconPath = U"Asset/loading_icon.png";  // ロード中アイコン画像のパス

  RotatingIcon() = default;

  // 初期化（Show と分離しているので、必要に応じて何度でも初期化可能）
  void Initialize(double iconSize,
                  const Vec2& iconPos,
                  double rotationSpeed = 180.0,
                  double rotationModulationAmplitude = 0.9,
                  double rotationModulationPeriod = 2.0,
                  double scaleModulationAmplitude = 0.2) {
    iconSize_ = iconSize;
    iconPos_ = iconPos;
    rotationSpeed_ = rotationSpeed;
    rotationModulationAmplitude_ = rotationModulationAmplitude;
    rotationModulationPeriod_ = rotationModulationPeriod;
    scaleModulationAmplitude_ = scaleModulationAmplitude;
    iconTexture_ = Texture{kLoadingIconPath};
  }

  // 表示を開始する（テクスチャ読み込み、タイマーリセット）
  void Show() {
    isVisible_ = true;
    rotationAngle_ = 0.0;
    elapsedTime_ = 0.0;
  }

  void Hide() { isVisible_ = false; }

  [[nodiscard]] bool IsVisible() const noexcept { return isVisible_; }

  // 毎フレームの更新（呼び出し側でScene::DeltaTime()が進む）
  void Update() {
    if (!isVisible_) return;

    elapsedTime_ += Scene::DeltaTime();
    const double phase = (Math::TwoPi * elapsedTime_) / rotationModulationPeriod_;
    const double modulation = 1.0 + rotationModulationAmplitude_ * Math::Sin(phase);
    rotationAngle_ += rotationSpeed_ * modulation * Scene::DeltaTime();
    if (rotationAngle_ >= 360.0) {
      rotationAngle_ = std::fmod(rotationAngle_, 360.0);
    }
  }

  // 描画を行う
  void Draw() const {
    if (!isVisible_) return;

    // スケール変調
    const double phase = (Math::TwoPi * elapsedTime_) / rotationModulationPeriod_;
    const double scale = 1.0 + scaleModulationAmplitude_ * Math::Sin(phase);

    if (iconTexture_) {
      const double rad = Math::ToRadians(rotationAngle_);
      const Transformer2D transform{Mat3x2::Rotate(rad, iconPos_) * Mat3x2::Scale(scale, iconPos_)};
      iconTexture_.resized(iconSize_).drawAt(iconPos_);
    } else {
      Circle{iconPos_, (iconSize_ / 2) * static_cast<float>(scale)}.drawFrame(UiConst::kInputAreaThickness, UiConst::kInputAreaBorderColor);
    }

    // RotatingIcon はロゴを描画しない
  }

  private:
  // パラメータ
  String iconPath_;
  double iconSize_ = 60.0;
  Vec2 iconPos_{0, 0};
  double rotationSpeed_ = 180.0;
  double rotationModulationAmplitude_ = 0.9;
  double rotationModulationPeriod_ = 2.0;
  double scaleModulationAmplitude_ = 0.2;

  // 状態
  double rotationAngle_ = 0.0;
  double elapsedTime_ = 0.0;
  bool isVisible_ = false;

  // テクスチャ
  Texture iconTexture_;
};
