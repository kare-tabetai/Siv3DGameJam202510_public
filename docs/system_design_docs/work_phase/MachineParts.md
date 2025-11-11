# MachineParts設計書

## 概要

工場で流れてくる部品の描画と動作を管理するクラス。六角ボルトを配置し、全てのボルトが締められたかどうかを判定する。

## 目的・スコープ

アルバイトフェーズにおける部品オブジェクトを表現する。3Dモデルとして直方体の本体と2～4個のボルトで構成され、画面左から右へ流れる動きを管理する。個別のボルトの描画や入力処理はHexBoltクラスが担当。

## ほかのクラスとの関係

- `WorkPhase` - このクラスのインスタンスを保持し、更新・描画を行う
- `HexBolt` - このクラスがボルトのインスタンス配列を保持し、管理する

## このクラスが継承するクラス

継承なし

## このクラスのコンストラクタ

```cpp
MachineParts(const Vec3& startPos)
```

position_=startPos、material_={kMetalColor}、metalTexture_={kMetalTexturePath}で初期化し、GenerateBolts()でボルトを生成する。移動速度はUpdate()呼び出し側が引数で渡す設計（speed_メンバは存在しない）。ボルトの数と配置位置はランダムに決定する。

## このクラスのデストラクタ

デフォルトデストラクタ

## このクラスに含まれるデータメンバ

- `Vec3 position_` - 部品の3D空間上の位置
- `Array<HexBolt> bolts_` - 配置されたボルトの配列（2～4個）
- `PhongMaterial material_` - 金属マテリアル
- `Texture metalTexture_` - 部品本体に貼る金属テクスチャ（ロード済みの場合はテクスチャで描画する）

## このクラスに含まれる定数

- `static constexpr Vec3 kBodySize{200.0, 20.0, 100.0}` - 部品本体のサイズ（幅、高さ、奥行き）
- `static constexpr int32 kMinBoltCount = 2` - 最小ボルト数
- `static constexpr int32 kMaxBoltCount = 4` - 最大ボルト数
- `static constexpr ColorF kMetalColor{0.6, 0.6, 0.65}` - 金属の色
- `static constexpr StringView kMetalTexturePath = U"Asset/metal_roung_tex.png"` - 金属テクスチャのパス

## このクラスに含まれるpublicメソッド

- `void Update(double deltaTime, const BasicCamera3D& camera, double conveyorSpeed)` - delta=Vec3{conveyorSpeed*deltaTime, 0.0, 0.0}で移動量計算、position_+=deltaで位置更新。各ボルトをbolt.Update(camera, delta)で更新してボルトが部品と同期移動
- `void Draw() const` - body{position_, kBodySize}で直方体作成。metalTexture_が有効ならbody.draw(metalTexture_)、無効ならbody.draw(material_)で部品本体描画。各ボルトをbolt.Draw()で描画
- `[[nodiscard]] bool IsCompleted() const noexcept` - std::all_of(bolts_.begin(), bolts_.end(), [](const HexBolt& bolt){return bolt.IsTightened();})で全ボルトが締められているかを判定
- `[[nodiscard]] double GetLeftEdge() const noexcept` - position_.x - kBodySize.x / 2で部品の左端X座標を返す（WorkPhaseで範囲外判定に使用）

## このクラスに含まれるprivateメソッド

- `void GenerateBolts()` - boltCount=Random(kMinBoltCount, kMaxBoltCount)でボルト数決定。randm_area=Point{kBodySize.x-HexBolt::kBoltRadius*2, kBodySize.z-HexBolt::kBoltRadius*2}で配置可能領域計算。boltCount回ループしてboltPos=Vec3{position_.x+Random(-randm_area.x/2, randm_area.x/2), position_.y+kBodySize.y/2+HexBolt::kBoltHeight/2, position_.z+Random(-randm_area.y/2, randm_area.y/2)}でボルト位置をランダム決定、bolts_ << HexBolt{boltPos}でボルトを配列に追加。重複チェックや最小距離保証のロジックは未実装

## このクラスで参照するアセットの情報

なし（プログラムで生成されるメッシュとマテリアルを使用）

## このクラスが参照する仕様書の項目

- アルバイトフェーズ
- 部品について

## このクラスが使用されるフェーズ

アルバイトフェーズ


## 特記事項・メモ

- 部品本体は直方体（Box型）で表現し、PhongMaterialで金属マテリアル設定
- 金属テクスチャ（Asset/metal_roung_tex.png）が読み込まれていればテクスチャ描画、失敗時はマテリアルのみで描画
- ボルトの配置位置は部品上面（Y軸プラス方向）にランダム配置。GenerateBolts()内でHexBolt::kBoltRadius*2を考慮した配置可能領域を計算してランダム位置決定
- 現在の実装では重複チェックや最小距離保証のロジックは未実装（将来的な改善余地あり）
- 全てのボルトが締められた（bolt.IsTightened()==true）場合にIsCompleted()がtrueを返す
- 画面外判定はこのクラスでは行わず、WorkPhase側でGetLeftEdge()の値とGameConst::kWorkAreaWidth/2を比較して判定
- 移動速度はUpdate()のconveyorSpeed引数で外部から制御される設計（speed_メンバは存在しない）
- Update()メソッドはカメラ情報と移動量（delta）をボルトに渡して同期移動を実現
