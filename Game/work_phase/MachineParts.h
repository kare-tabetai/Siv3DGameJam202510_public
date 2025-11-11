// MachineParts.h
// 工場で流れてくる部品の描画と動作を管理するクラス

#pragma once
#include <Siv3D.hpp>

#include "HexBolt.h"

// 工場で流れてくる部品の描画と動作を管理するクラス
// 六角ボルトを配置し、全てのボルトが締められたかどうかを判定する
class MachineParts {
  public:
  // コンストラクタ
  // 速度は外部から毎フレーム渡されるようになったため、コンストラクタで受け取らない
  MachineParts(const Vec3& startPos)
      : position_(startPos), material_{kMetalColor}, metalTexture_{kMetalTexturePath} {
    // マテリアルは色で初期化しつつ、金属テクスチャを読み込む

    // ボルトを生成
    GenerateBolts();
  }

  // 毎フレーム呼ばれる更新処理
  // 位置を移動し、ボルトを更新する。カメラ情報は各ボルトのUpdate()に渡す
  // 追加の引数 `conveyorSpeed` を受け取り、ベルトのスクロール速度で部品を移動させる
  void Update(double deltaTime, const BasicCamera3D& camera, double conveyorSpeed) {
    // 位置を右に移動（conveyorSpeed によって移動量を決定）
    const Vec3 delta = Vec3{conveyorSpeed * deltaTime, 0.0, 0.0};
    position_ += delta;

    // 各ボルトを更新（ボルトの位置は部品本体からの相対位置なので、部品の移動量を渡す）
    for (auto& bolt : bolts_) {
      bolt.Update(camera, delta);
    }
  }

  // 部品本体とボルトを描画する
  void Draw() const {
    // 部品本体（直方体）を描画
    const Box body{position_, kBodySize};
    // テクスチャが有効に読み込まれていればテクスチャで描画、そうでなければフォングマテリアルで描画
    if (metalTexture_) {
      body.draw(metalTexture_);
    } else {
      body.draw(material_);
    }

    // 各ボルトを描画
    for (const auto& bolt : bolts_) {
      bolt.Draw();
    }
  }

  // 全てのボルトが締められたかどうかを判定する
  [[nodiscard]] bool IsCompleted() const noexcept {
    // 全てのボルトが締められていればtrue
    return std::all_of(bolts_.begin(), bolts_.end(),
                       [](const HexBolt& bolt) { return bolt.IsTightened(); });
  }

  // 部品の左端のX座標を取得する
  [[nodiscard]] double GetLeftEdge() const noexcept {
    return position_.x - kBodySize.x / 2;
  }

  // 定数
  static constexpr Vec3 kBodySize{200.0, 20.0, 100.0};  // 部品本体のサイズ（幅、高さ、奥行き）
  static constexpr int32 kMinBoltCount = 2;             // 最小ボルト数
  static constexpr int32 kMaxBoltCount = 4;             // 最大ボルト数
  static constexpr ColorF kMetalColor{0.6, 0.6, 0.65};  // 金属の色

  // 金属テクスチャのパス
  static constexpr StringView kMetalTexturePath = U"Asset/metal_roung_tex.png";

  private:
  // ボルトをランダムに2～4個生成し、部品上面にランダムに配置する
  void GenerateBolts() {
    // ボルト数をランダムに決定
    const int32 boltCount = Random(kMinBoltCount, kMaxBoltCount);

    Point randm_area = Point{
      static_cast<int>(kBodySize.x - HexBolt::kBoltRadius * 2),
      static_cast<int>(kBodySize.z - HexBolt::kBoltRadius * 2)};

    // ボルトを生成
    for (int32 i = 0; i < boltCount; ++i) {
      Vec3 boltPos = Vec3{
        position_.x + Random(-randm_area.x / 2, randm_area.x / 2),
        position_.y + kBodySize.y / 2 + HexBolt::kBoltHeight / 2,
        position_.z + Random(-randm_area.y / 2, randm_area.y / 2)};

      // ボルトを配列に追加
      bolts_ << HexBolt{boltPos};
    }
  }

  // データメンバ
  Vec3 position_;           // 部品の3D空間上の位置
  // speed_ は削除: 移動速度は update 呼び出し側から渡される
  Array<HexBolt> bolts_;    // 配置されたボルトの配列（2～4個）
  PhongMaterial material_;  // 金属マテリアル
  Texture metalTexture_;    // 部品本体に貼る金属テクスチャ
};
