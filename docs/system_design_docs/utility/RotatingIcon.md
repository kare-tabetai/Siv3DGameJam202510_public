# RotatingIcon設計書

## 概要
回転するローディングアイコンを担当するヘッダオンリーのユーティリティクラス。テクスチャの読み込み、回転・スケール変調、描画までをカプセル化する。

## 目的・スコープ
ロード中やサーバー通信中などに表示する回転アニメーションアイコンを提供する。回転速度、変調、スケール変化を設定可能。アイコンの表示/非表示の制御も行う。

## ほかのクラスとの関係
- `LoadingUI` - このクラスを使用してロード中のアイコンを表示
- `ServerLoadingUI` - このクラスを使用してサーバー通信中のアイコンを表示
- `UiConst` - UI共通の定数値を参照する

## このクラスが継承するクラス
なし

## このクラスのコンストラクタ
- デフォルトコンストラクタのみ

## このクラスのデストラクタ
- デフォルトデストラクタのみ

## このクラスに含まれるデータメンバ
- `String iconPath_` - アイコン画像のファイルパス
- `double iconSize_` - アイコンのサイズ（ピクセル）、デフォルトは60.0
- `Vec2 iconPos_` - アイコンの表示位置、デフォルトは(0, 0)
- `double rotationSpeed_` - 回転速度（度/秒）、デフォルトは180.0
- `double rotationModulationAmplitude_` - 回転速度の変調振幅、デフォルトは0.9
- `double rotationModulationPeriod_` - 回転速度の変調周期（秒）、デフォルトは2.0
- `double scaleModulationAmplitude_` - スケール変調の振幅、デフォルトは0.2
- `double rotationAngle_` - 現在の回転角度（度）、デフォルトは0.0
- `double elapsedTime_` - 経過時間（秒）、デフォルトは0.0
- `bool isVisible_` - 表示中かどうかのフラグ、デフォルトはfalse
- `Texture iconTexture_` - アイコンのテクスチャ

## このクラスに含まれる定数
- `static inline const String kLoadingIconPath = U"Asset/loading_icon.png"` - ロード中アイコン画像のパス

## このクラスに含まれるpublicメソッド
- `void Initialize(double iconSize, const Vec2& iconPos, double rotationSpeed = 180.0, double rotationModulationAmplitude = 0.9, double rotationModulationPeriod = 2.0, double scaleModulationAmplitude = 0.2)` - 初期化処理。アイコンサイズ、位置、回転速度などを設定する
- `void Show()` - アイコンの表示を開始する。タイマーをリセットし、回転角度を0にする
- `void Hide()` - アイコンの表示を停止する
- `[[nodiscard]] bool IsVisible() const noexcept` - アイコンが表示中かどうかを返す
- `void Update()` - 毎フレームの更新処理。回転角度とスケール変調を計算する
- `void Draw() const` - アイコンを描画する。テクスチャがあれば描画し、なければ円を描画する

## このクラスに含まれるprivateメソッド
なし

## このクラスで参照するアセットの情報
- アイコン画像: `Asset/loading_icon.png`

## このクラスが参照する仕様書の項目
- 求職活動フェーズのローディング画面
- ローディングUI

## イメージ図
無し

## このクラスが使用されるフェーズ
- 求職活動フェーズ

## 特記事項・メモ
- ヘッダーオンリーで実装される
- テクスチャが読み込めない場合は円で代替表示する
- 回転速度とスケールはサイン波で変調される
