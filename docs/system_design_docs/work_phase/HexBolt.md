# HexBolt設計書

## 概要

六角ボルトの描画と管理を行うクラス。クリック判定を検知し、締められたかどうかの状態を保持する。

## 目的・スコープ

アルバイトフェーズにおけるボルトオブジェクトを表現する。3D空間上の位置とクリック可能な領域を持ち、プレイヤーのクリックで締める動作を実現する。ボルトを締める音の再生も担当する。

## ほかのクラスとの関係

- `MachineParts` - このクラスのインスタンス配列を保持する
- `WorkPhase` - 間接的に（MachinePartsを通じて）このクラスを使用する
- `SoundManager` - ボルトを締める音（レンチ音）の再生を要求する

## このクラスが継承するクラス

継承なし

## このクラスのコンストラクタ

```cpp
explicit HexBolt(const Vec3& position)
```

position_=position、isTightened_=falseで初期化。boltMesh_=Mesh{MeshData::Cylinder({0, 0, 0}, kBoltRadius, kBoltHeight, 6)}で六角柱メッシュ生成、material_=PhongMaterial{kDefaultColor}でマテリアル初期化。

## このクラスのデストラクタ

デフォルトデストラクタ

## このクラスに含まれるデータメンバ

- `Vec3 position_` - ボルトの3D空間上の位置（初期値コンストラクタ引数）
- `Quaternion rotation_ = Quaternion::Identity()` - 現在の回転（アニメーション用、初期値Identity）
- `bool isTightened_ = false` - 締められたかどうか（初期値false）
- `bool isAnimating_ = false` - 締めるアニメーション中か（初期値false）
- `double animationElapsed_ = 0.0` - アニメーション経過時間(秒、初期値0.0)
- `Mesh boltMesh_` - ボルトのメッシュ（六角柱、コンストラクタで生成）
- `PhongMaterial material_` - マテリアル（コンストラクタでkDefaultColorで初期化）

## このクラスに含まれる定数
`static constexpr double kBoltRadius = 10.0` - ボルトの半径

`static constexpr double kBoltHeight = 5.0` - ボルトの高さ

`static constexpr double kClickRadius = 20.0` - クリック判定の半径（ボルトより少し大きめ）

`static constexpr double kAnimationDuration = 0.4` - クリックから締め終わりまでのアニメーション時間(秒)

`static constexpr double kTargetRotation = Math::TwoPi * 2` - 締める時の目標回転（ラジアン）

`static inline const ColorF kDefaultColor{0.4, 0.4, 0.45}` - デフォルトの色（灰色）

`static inline const ColorF kTightenedColor{0.6, 0.6, 0.65}` - 締めた後の色（明るい灰色）

## このクラスに含まれるpublicメソッド

- `void Update(const BasicCamera3D& camera, const Vec3& parentDelta)` - position_+=parentDeltaで親の移動量を反映。isAnimating_がtrueならanimationElapsed_+=Scene::DeltaTime()、progress=Min(animationElapsed_/kAnimationDuration, 1.0)、eased=TweenUtil::EaseOutCubic(progress)、rotation_=Quaternion::RotateY(-eased*kTargetRotation)で回転アニメーション更新。progress>=1.0でisAnimating_=false、isTightened_=true、material_=PhongMaterial{kTightenedColor}でアニメーション完了。アニメーション中は早期リターン。isTightened_がtrueなら早期リターン。IsMouseOver(camera) && MouseL.down()でOnClick()呼び出し
- `void Draw() const` - boltMesh_.draw(position_, rotation_, material_)でボルトを描画（回転と位置を反映）
- `[[nodiscard]] bool IsTightened() const noexcept` - isTightened_を返す
- `[[nodiscard]] Vec3 GetPosition() const noexcept` - position_を返す

## このクラスに含まれるprivateメソッド

- `void OnClick()` - isTightened_ || isAnimating_なら早期リターン。isAnimating_=true、animationElapsed_=0.0、rotation_=Quaternion::Identity()でアニメーション開始、SoundManager::PlaySE(U"se_wrench")でレンチ音再生
- `[[nodiscard]] bool IsMouseOver(const BasicCamera3D& camera) const` - ray=camera.screenToRay(Cursor::Pos())でレイ生成、clickSphere={position_, kClickRadius}で球体作成、ray.intersects(clickSphere).has_value()で交差判定してマウスがボルト上にあるか判定
- `[[nodiscard]] Vec2 GetScreenPosition(const BasicCamera3D& camera) const` - 3D位置を2Dスクリーン座標に変換するスタブ実装（現在未使用、将来用）。(void)camera; return Vec2{0, 0};

## このクラスで参照するアセットの情報

- SE: `Asset/wrench-02-101112.mp3` (SoundManager の `se_wrench`) - レンチでボルトを締める音

## 実装上のメモ（ヘッダに合わせた補足）

- `boltMesh_` は六角柱を表現するために MeshData::Cylinder(..., 6) を用いて生成される
- 回転は `Quaternion rotation_` で保持され、ツウィーン（EaseOutCubic）で `rotation_` を更新して描画に反映する
- アニメーション中はクリック判定を無効化する実装になっている
- `GetScreenPosition()` は将来の用途に備えたスタブとして残されている（未使用）

## このクラスが参照する仕様書の項目

- アルバイトフェーズ
- 部品について

## このクラスが使用されるフェーズ

アルバイトフェーズ

## 特記事項・メモ

- ボルトは六角柱のメッシュで表現（MeshData::Cylinder(..., 6)で6分割して生成）
- クリック判定はレイキャストで実行。camera.screenToRay()でマウス位置からレイを生成し、Sphere{position_, kClickRadius}との交差判定で検出
- Update()はparentDeltaを受け取りposition_+=parentDeltaで部品とともに移動
- 締めるアニメーションはTweenUtil::EaseOutCubic()で回転を滑らかに変化させ、0.4秒間で2回転（Math::TwoPi*2ラジアン）
- アニメーション完了時にisTightened_=true、material_=PhongMaterial{kTightenedColor}で色を明るい灰色に変更
- 締められた状態またはアニメーション中はクリック判定を受け付けない（OnClick()で早期リターン、Update()でアニメーション中は早期リターン）
- GetScreenPosition()は将来の拡張用スタブとして残されている（現在未使用）
