# MessageWindowUI設計書

## 概要

メッセージウィンドウの描画と管理を行うクラス。主人公のセリフや地の文を画面下部に表示し、プレイヤーのクリックで次のメッセージに進める。モノステートパターンで実装され、どこからでもアクセス可能。

## 目的・スコープ

ノベルゲーム的なテキスト表示を実現する。文字を徐々に表示するタイプライター効果、複数メッセージの管理、表示完了判定などを担当する。モノステートパターンにより、インスタンス化せずに静的メソッドで利用可能。スキップやオート進行機能は実装しない（仕様書の要件により）。

## ほかのクラスとの関係

- `GameManager` - ゲーム開始時にこのクラスのInitialize()を呼び出し、毎フレームUpdate()とDraw()を呼び出す
- 各フェーズクラス（App::ISceneを継承） - メッセージを設定してこのUIを表示する
- `FontManager` - UI表示のフォントを取得する

## このクラスが継承するクラス

継承なし（モノステートパターンで実装）

## このクラスのコンストラクタ

デフォルトコンストラクタのみ。実際の初期化は`Initialize()`メソッドで行う。

## このクラスのデストラクタ

デフォルトデストラクタ

## このクラスに含まれるデータメンバ

- `static inline Array<String> messages_` - 表示するメッセージ配列（初期値空配列）
- `static inline int32 currentIndex_ = 0` - 現在表示中のメッセージインデックス（初期値0）
- `static inline bool isVisible_ = false` - 表示中かどうか（初期値false）
- `static inline RectF windowRect_` - ウィンドウの矩形（画面下kWindowHeightRatio比率）
- `static inline int32 currentCharIndex_ = 0` - 現在表示中の文字インデックス（初期値0）
- `static inline Stopwatch charTimer_` - 文字表示用タイマー
- `static inline ColorF textColor_ = kTextColor` - テキストの色（初期値白色）
- `static inline bool clickToAdvance_ = true` - クリックで次へ進むかどうか（初期値true）

## このクラスに含まれる定数

- `static constexpr double kCharDisplayInterval = 0.03` - 1文字表示間隔（秒）
- `static constexpr double kWindowHeightRatio = 0.25` - ウィンドウの高さ比率（画面の1/4）
- `static constexpr double kTextMargin = 30.0` - テキストのマージン
- `static constexpr ColorF kWindowBgColor{0.0, 0.0, 0.0, 0.9}` - ウィンドウ背景色（濃い半透明の黒）
- `static constexpr ColorF kTextColor{1.0, 1.0, 1.0}` - テキスト色（白色）
- `static constexpr int32 kFontSize = 24` - フォントサイズ
- `static constexpr double kMarkerSize = 20.0` - 次メッセージマーカー（三角形）のサイズ
- `static constexpr double kMarkerAmplitude = 10.0` - マーカーの上下揺れ幅（ピクセル）
- `static constexpr double kMarkerFrequency = 2.0` - マーカーの揺れ周波数（Hz）

## このクラスに含まれるpublicメソッド

- `static void Initialize()` - 静的メンバ変数を初期化。messages_.clear()、currentIndex_=0、isVisible_=false、currentCharIndex_=0、charTimer_.reset()、textColor_=kTextColor。windowRect_をScene::Height() * kWindowHeightRatio使って画面下部に設定
- `static void Show(const Array<String>& messages)` - メッセージを設定して表示を開始。messages_=messages、currentIndex_=0、isVisible_=true、currentCharIndex_=0、charTimer_.restart()
- `static void Hide()` - メッセージウィンドウを非表示にし、内部状態をリセット。isVisible_=false、currentIndex_=0、currentCharIndex_=0、charTimer_.reset()
- `static bool Next()` - 次のメッセージへ進む。文字表示中ならcurrentCharIndex_を全文長に設定、表示完了ならcurrentIndex_++で次へ。最後のメッセージならHide()呼び出してfalseを返す。それ以外はtrueを返す
- `static void Update()` - clickToAdvance_がtrueでMouseL.down()ならNext()呼び出し。文字のタイプライター表示更新：charTimer_.sF() >= kCharDisplayIntervalでcurrentCharIndex_++とcharTimer_.restart()
- `static void Draw()` - メッセージウィンドウを描画。windowRect_.draw(kWindowBgColor)で背景、FontManager::GetFont(U"ui_medium")でフォント取得してdisplayText描画。currentCharIndex_ >= currentMessage.length()ならDrawNextMarker()呼び出し
- `static [[nodiscard]] bool IsVisible() noexcept` - isVisible_を返す
- `static [[nodiscard]] bool IsCompleted() noexcept` - !isVisible_ && !messages_.isEmpty()を返す
- `static void SetTextColor(const ColorF& color) noexcept` - textColor_=colorで色変更
- `static [[nodiscard]] ColorF GetTextColor() noexcept` - textColor_を返す
- `static void SetClickToAdvance(bool enabled) noexcept` - clickToAdvance_=enabledでクリック進行有効化/無効化

## このクラスに含まれるprivateメソッド

- `static void DrawNextMarker()` - 次メッセージマーカー（三角形）を描画。Math::Sin(Scene::Time() * Math::TwoPi * kMarkerFrequency) * kMarkerAmplitudeで上下に揺れるアニメーション。ウィンドウ右下のmarkerPos位置にTriangle{markerPos, markerPos + Vec2{kMarkerSize, 0}, markerPos + Vec2{kMarkerSize / 2, kMarkerSize}}を下向き三角形として描画、textColor_で着色

## このクラスで参照するアセットの情報

- フォント: `Asset/IBM_Plex_Sans_JP/IBMPlexSansJP-Regular.ttf`

## このクラスが参照する仕様書の項目

- 基本操作
- 共通UI
- メッセージウィンドウ
- 各フェーズ（メッセージ表示が必要な全フェーズ）

## イメージ図

![共通UI/メッセージウィンドウのイメージ図](../spec_image/message_window.png)

## このクラスが使用されるフェーズ

全フェーズで使用可能（必要に応じて各フェーズから呼び出される）

## 特記事項・メモ

- モノステートパターンで実装し、静的メンバのみを使用する
- 文字は1文字ずつ0.03秒間隔で表示される（タイプライター効果）
- クリック時の動作：
  - 文字表示中 → 即座に全文表示
  - 全文表示完了 → 次のメッセージへ
  - 最後のメッセージ表示完了 → Hide()を呼び出して非表示に
- 次メッセージマーカーは Sin 関数で上下に揺れるアニメーション
- テキストは自動改行対応（font_.draw()の幅指定機能を使用）
- フォントはFontManagerから使用時に取得する（メンバとして保持しない）
- ウィンドウサイズは画面サイズに応じて動的に計算される
- ゲーム開始時に`Initialize()`を必ず呼び出す必要がある
