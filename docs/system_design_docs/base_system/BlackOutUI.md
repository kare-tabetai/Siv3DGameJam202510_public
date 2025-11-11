# BlackOutUI設計書

## 概要

暗転処理と暗転時に表示するメッセージを管理するモノステートクラス。画面全体を徐々に暗く（フェードイン）/明るく（フェードアウト）し、必要に応じて中央にメッセージを表示する。静的メソッドと静的データメンバのみで実装され、どこからでもアクセス可能。

## 目的・スコープ

シーン遷移や演出（例：導入、エンディング、バッドエンド等）における暗転表現を提供する。フェード制御（開始、進行、完了判定）と暗転中の簡易メッセージ表示を担当する。複雑な UI レイアウトやアニメーションは対象外。

## ほかのクラスとの関係

- `GameManager` - 起動時に `Initialize()` を呼び、毎フレーム `Update()` と `Draw()` を呼び出す想定
- `FontManager` - 描画時にフォントを取得するため依存（`FontManager::GetFont(U"ui_large")` を使用）
- 各フェーズクラス - フェーズ遷移や演出から `FadeIn`/`FadeOut`/`SetMessage` 等を呼び出す

## 継承

継承なし（モノステート実装）

## コンストラクタ / デストラクタ

コンストラクタ・デストラクタは削除されており、インスタンス化しない設計。状態の初期化は `Initialize()` で行う。

## データメンバ

- `static inline String message_` - 暗転時に表示するメッセージ（初期値空文字列）
- `static inline double fadeAlpha_ = 0.0` - フェードのアルファ値（0.0～1.0、初期値0.0）
- `static inline State state_ = State::Hidden` - 現在の状態（Hidden/FadingIn/Visible/FadingOut、初期値Hidden）
- `static inline double fadeDuration_ = kDefaultFadeDuration` - フェードの所要時間（秒、初期値1.0）
- `static inline Stopwatch fadeTimer_` - フェードタイマー
- `static inline ColorF textColor_ = kTextColor` - 実行時に変更可能なテキスト色（初期値白色）

## 定数

- `static constexpr ColorF kBlackOutColor{0.0, 0.0, 0.0}` - 暗転の色（黒）
- `static constexpr int32 kFontSize = 32` - （参照用）メッセージ表示のフォントサイズ
- `static constexpr ColorF kTextColor{1.0, 1.0, 1.0}` - テキスト色（白）
- `static constexpr double kDefaultFadeDuration = 1.0` - デフォルトのフェード時間（秒）

## public メソッド

- `static void Initialize()`
  - 役割: message_.clear()、fadeAlpha_=0.0、state_=State::Hidden、fadeDuration_=kDefaultFadeDuration、fadeTimer_.reset()で内部状態を初期化。**フォントやサウンド等の外部リソースはここで初期化しない**。FontManager等の依存は外部（`GameManager`等）で先に初期化されている必要がある。

- `static void FadeIn(double duration = kDefaultFadeDuration)` - state_=State::FadingIn、fadeDuration_=duration、fadeTimer_.restart()で暗転（徐々に黒く）を開始
- `static void FadeOut(double duration = kDefaultFadeDuration)` - state_=State::FadingOut、fadeDuration_=duration、fadeTimer_.restart()で明転（徐々に明るく）を開始
- `static void SetMessage(const String& message)` - message_=messageで暗転中に中央へ表示するメッセージを設定
- `static void SetTextColor(const ColorF& color)` - textColor_=colorでテキスト色を設定
- `static void ResetTextColor()` - textColor_=kTextColorでテキスト色をデフォルト（白色）に戻す
- `static void ClearMessage()` - message_.clear()でメッセージをクリア
- `static void ShowImmediately()` - state_=State::Visible、fadeAlpha_=1.0、fadeTimer_.reset()で即座に完全表示
- `static void HideImmediately()` - state_=State::Hidden、fadeAlpha_=0.0、fadeTimer_.reset()で即座に非表示
- `static void Update()` - State::FadingIn時はfadeTimer_.sF()/fadeDuration_でfadeAlpha_計算、progress>=1.0でState::Visibleへ遷移。State::FadingOut時は1.0-progressでfadeAlpha_計算、progress>=1.0でState::Hiddenへ遷移
- `static void Draw()` - state_==State::Hiddenなら何もしない。Scene::Rect().draw(ColorF{kBlackOutColor, fadeAlpha_})で画面全体を黒で描画。message_.isEmpty()でないならFontManager::GetFont(U"ui_large")取得、font(message_).draw(Arg::center=Scene::Center(), ColorF{textColor_, fadeAlpha_})で中央に描画
- `[[nodiscard]] static bool IsFading() noexcept` - state_==State::FadingIn || state_==State::FadingOutを返す
- `[[nodiscard]] static bool IsVisible() noexcept` - state_==State::Visibleを返す
- `[[nodiscard]] static bool IsHidden() noexcept` - state_==State::Hiddenを返す
- `[[nodiscard]] static double GetAlpha() noexcept` - fadeAlpha_を返す

## private メソッド

なし

## 参照アセット / 依存関係

- `FontManager::GetFont(U"ui_large")` を利用してメッセージを描画するため、`FontManager::Initialize()` が先に呼ばれていることが前提（実際は `GameManager` が起動時に順序を担保している）。

## 参照する仕様書の項目

- 導入フェーズ
- 一日の終わりフェーズ
- エンディングフェーズ
- ゲームオーバーフェーズ
- バッドエンドフェーズ

## 使用されるフェーズ

全フェーズで使用可能。主にフェーズ遷移時・演出時に使用される。

## 特記事項・メモ

- 実装はモノステート（静的メンバ）で、インスタンス生成は行わない。
- フェードの進行は `Stopwatch` で計測し、線形補間でアルファを計算している。
- メッセージは中央揃えで表示され、描画アルファは暗転のアルファと同一である。
- `Initialize()` は内部状態のリセットのみに限定し、外部リソース（フォント等）の初期化は行わない点に注意。
- `GameManager` は起動時に `FontManager::Initialize()` を呼び、`BlackOutUI::Initialize()` を続けて呼び出す設計になっているため、実行順序に問題がないことを前提とする。

