# WorkPhase設計書

## 概要

アルバイトフェーズを管理するクラス。流れてくる部品のボルトを締める作業を行う。

## 目的・スコープ

「モダンタイムズ」のように、主人公が社会にとって代替可能な部品として扱われている虚しさを感じさせることが目的。左から流れてくる部品の六角ボルトをクリックで締め、5個分完了させるゲームプレイを管理する。ゲーム性は重視しない。

## ほかのクラスとの関係

- `GameManager` - このクラスのインスタンスを管理し、更新・描画を行う
- `MachineParts` - 部品のインスタンスを保持し、管理する
- `SunsetPhase` - アルバイト完了後、このフェーズへ遷移する
- `MessageWindowUI` - 初回時の操作説明表示に使用する
- `GameCommonData` - ゲーム共通データを参照する
- `SoundManager` - 工場環境音BGMを再生する

## このクラスが継承するクラス

`iPhase`を継承する

## このクラスのコンストラクタ

```cpp
explicit WorkPhase()
```

デフォルトコンストラクタ。コンストラクタ内でカメラと光源の初期化（SetupCamera()）、初回プレイ時の操作説明表示の開始判定、BGM再生（SoundManager::PlayBGM(U"bgm_factory")）、背景の設定（BackGroundManager::SetBackGround(U"")）および最初の部品生成（SpawnNewParts()）を行う。

## このクラスのデストラクタ

デフォルトデストラクタ

## このクラスに含まれるデータメンバ

- `Optional<MachineParts> currentParts_` - 現在流れている部品（存在しない場合はnone）
- `int32 completedCount_` - 完了した部品数（初期値0）
- `State state_` - 現在の状態（ShowingInstruction/Running/EndMessage/EndFadeWait、初期値Running）
- `BasicCamera3D camera_` - 3D描画用のカメラ
- `Mesh planeMesh_` - 床用メッシュ
- `Texture checkerTexture_` - チェッカーボードのテクスチャ
- `DynamicMesh beltMesh_` - ベルト用メッシュ（繰り返し表示用）
- `MeshData beltMeshData_` - ベルトの元メッシュデータ（UVを書き換えるために保持）
- `Texture beltTexture_` - ベルトコンベアのテクスチャ
- `Texture metalTexture_` - ベルト淵用の金属テクスチャ
- `double scrollSpeed_` - ベルトのスクロール速度（初期値kInitialScrollSpeed）
- `double uvOffset_` - ベルトのUVオフセット（スクロール位置、初期値0.0）

## このクラスに含まれる定数

### 状態定義

- `enum class State` - フェーズ内の状態を表す列挙型
  - `ShowingInstruction` - 操作説明表示中
  - `Running` - 作業中
  - `EndMessage` - 終了メッセージを表示して完了を待つ
  - `EndFadeWait` - 暗転フェードの終了を待つ

### ゲームプレイ関連

- `static constexpr int32 kTargetCompletedCount = 5` - 目標完了数
- `static constexpr double kInitialScrollSpeed = 200.2` - ベルトのスクロール初期速度
- `static constexpr Vec3 kPartsStartPos{-GameConst::kWorkAreaWidth/2, 0.0, 0.0}` - 部品の開始位置（画面左外）

### カメラ・光源関連

- `static constexpr Vec3 kCameraPos{0.0, 150.0, -75.0}` - カメラの位置
- `static constexpr Vec3 kCameraTarget{0.0, 0.0, 0.0}` - カメラの注視点
- `static constexpr double kCameraFOV = 80_deg` - カメラの視野角
- `static constexpr ColorF kAmbientLightColor{0.35, 0.35, 0.35}` - 環境光の色
- `static constexpr ColorF kSunColor{1.0, 0.98, 0.95}` - 平行光源の色（工場の蛍光灯を想定した白色）
- `static constexpr Vec3 kSunLightDirection{0.0, -1.0, 0.3}` - 平行光源の方向

### ベルトコンベア関連

- `static constexpr Vec2 kBeltSize{GameConst::kWorkAreaWidth, MachineParts::kBodySize.z + 80}` - ベルトのサイズ（3次元的には(x,z)）
- `static constexpr StringView kBeltTexturePath = U"Asset/gom_tex.png"` - ベルトコンベアのテクスチャパス
- `static constexpr StringView kCheckerBoardTexturePath = U"Asset/checker_board_tex.png"` - チェッカーボードテクスチャのパス
- `static constexpr StringView kMetalBoardTexturePath = U"Asset/metal_board_tex.png"` - メタルボードテクスチャのパス

### 床(Plane)関連

- `static constexpr Vec2 kFloorSize{900.0, 900.0}` - 床のサイズ（カメラの見える範囲をカバーするために十分大きめに）
- `static constexpr double kFloorTileSize = 200.0` - テクスチャの1タイルあたりの大きさ (ワールド単位)
- `static constexpr double kFloorYOffset = 0.5` - 床プレーンのオフセット

### ベルトの淵関連

- `static constexpr double kRimThickness = 10.0` - ベルト淵の厚み（Z方向／ワールド単位）
- `static constexpr double kRimHeight = 10.0` - ベルト淵の高さ（Y方向の厚み）

## このクラスに含まれるpublicメソッド

- `void update() override` - 毎フレーム呼ばれる更新処理。ベルトUVのスクロール更新、状態ごとの更新分岐（操作説明中、実行中、終了メッセージ表示、暗転待ち）を行う。`Running` 状態では `currentParts_->Update(Scene::DeltaTime(), camera_, scrollSpeed_)` を呼び出して部品を更新し、`completedCount_ >= kTargetCompletedCount` のときは終了メッセージ表示へ遷移する
- `void draw() const override` - 描画処理。3D描画で部品を描画した後、2D UI（完了数、CommonUI）や操作説明ウィンドウを描画する

## このクラスに含まれるprivateメソッド

- `void SpawnNewParts()` - 新しい部品を生成して画面左から流す
- `void CheckPartsCompletion()` - 部品のボルト完了判定を行う。画面外判定（未完了扱い）は `WorkPhase` 側で行い、`MachineParts::GetLeftEdge()` が `GameConst::kWorkAreaWidth / 2` を超えた場合に範囲外と見なす
- `void ShowInstruction()` - 初回時の操作説明をMessageWindowUIに表示
- `void DrawCompletionCount() const` - 完了数を画面上部に描画
- `void SetupCamera()` - カメラの初期設定を行う。コンストラクタから呼び出される
- `void UpdateCamera()` - カメラの更新処理（必要に応じて）

- `MeshData CreateBeltMeshData() const` - ベルト用の MeshData を生成する内部ユーティリティ。UV オフセット（uvOffset_）を用いてテクスチャのスクロールを表現する
- `void UpdateRunningState()` - Running 状態の更新ロジック（部品更新、完了判定、完了到達時の BGM 停止とメッセージ表示）
- `void UpdateEndMessage()` - 終了メッセージ表示時の更新ロジック（メッセージ完了で暗転要求）
- `void UpdateEndFadeWait()` - 暗転フェード終了待ち。暗転が完了したら `PhaseManager::ChangePhase(PhaseType::Sunset)` を呼ぶ

## このクラスで参照するアセットの情報

- BGM: `Asset/factory_exh_fan.mp3` (SoundManager の `bgm_factory` に登録) - 工場内を示す環境音
- SE: `Asset/wrench-02-101112.mp3` (SoundManager の `se_wrench` に登録) - ボルトを締める音（HexBolt::OnClick で使用）
- UIフォント: `FontManager::GetFont(U"ui_medium")` を使用して完了数を描画

## このクラスが参照する仕様書の項目

- アルバイトフェーズ
- 部品について
- フェーズ遷移図

## イメージ図

![アルバイトフェーズのイメージ図](../spec_image/work_phase.png)

## このクラスが使用されるフェーズ

アルバイトフェーズ

## 特記事項・メモ

- 初回プレイ時のみ操作説明ウィンドウを表示する
  - 初回判定はGameCommonData.IsFirstDay()を使用
- 操作説明の例：
  - 「部品が左から流れてきます」
  - 「六角ボルトをクリックして締めてください」
  - 「全てのボルトを締めれば作業完了です」
- 部品は `MachineParts::GetLeftEdge()` の値が `GameConst::kWorkAreaWidth / 2` より大きくなった場合に範囲外（右側に流れ出た）と判定され、未完了扱いの場合は `completedCount` は増えない。範囲外判定は `WorkPhase` の責務である
- 完了数が `kTargetCompletedCount`（=5）に達したら自動的に終了処理（BGM停止→終了メッセージ→暗転→SunsetPhase）へ遷移する
- BGMはSoundManager::PlayBGM("bgm_factory")でループ再生し、フェーズ終了時にSoundManager::StopBGM()で停止する
- MessageWindowUIは`Array<String>`で複数メッセージを表示可能

### 3D描画設定について

- このフェーズのみ3D描画を使用する
- カメラは `BasicCamera3D{Scene::Size(), kCameraFOV, kCameraPos, kCameraTarget}` を用い、ヘッダ実装では視野角 `kCameraFOV = 80_deg`、位置 `kCameraPos = {0,150,-75}` で比較的近くから見下ろす配置になっている
- `Graphics3D::SetGlobalAmbientColor(kAmbientLightColor)` で環境光を設定し、暗めの工場らしいトーンにする（推奨: `ColorF{0.35,0.35,0.35}`）
- ベルトは `DynamicMesh` として `CreateBeltMeshData()` により生成した MeshData を `beltMesh_.fill(md)` で更新し、テクスチャは `beltTexture_` を使用してスクロール描画する
- draw() メソッド内では以下の順序で描画を行う：
  1. Scene::SetBackground(ColorF{0.3,0.3,0.35}) で背景色を設定
  2. Graphics3D の環境光・カメラを設定
  3. 床（planeMesh_）→ベルト（beltMesh_.draw(beltTexture_)）→ベルト淵（Box で金属テクスチャ描画）→部品（currentParts_->Draw()）の順で 3D 要素を描画
  4. 2D UI（完了数、CommonUI、MessageWindowUI）を描画
