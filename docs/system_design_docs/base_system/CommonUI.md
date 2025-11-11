# CommonUI設計書

## 概要

GameCommonDataの要素（日付、残り日数、精神力）を画面上に常時表示するUIクラス。モノステートパターンで実装され、どこからでもアクセス可能。

## 目的・スコープ

ゲーム進行中、プレイヤーが現在の状態を常に把握できるように共通UIを描画する。画面上部に配置し、日付、残り日数、精神力を表示する。モノステートパターンにより、インスタンス化せずに静的メソッドで利用可能。メッセージウィンドウやフェーズ固有のUIは対象外。

## ほかのクラスとの関係

- `GameManager` - ゲーム開始時にこのクラスのInitialize()を呼び出し、毎フレームDraw()を呼び出す
- `GameCommonData` - このクラスの静的メソッドを呼び出してデータを取得し、表示内容を決定する
- `FontManager` - UI表示のフォントを取得する

## このクラスが継承するクラス

継承なし（モノステートパターンで実装）

## このクラスのコンストラクタ

コンストラクタは削除（`CommonUI() = delete`）。モノステートのため不要

## このクラスのデストラクタ

モノステートのため不要

## このクラスに含まれるデータメンバ

なし（全て静的定数）

## このクラスに含まれる定数

- `static constexpr RectF kBgRect{0, 0, GameConst::kWindowSize.x, GameConst::kCommonUIHeight}` - 背景矩形の位置とサイズ（画面上部、GameConst::kCommonUIHeight高さ）
- `static constexpr ColorF kBgColor{0.0, 0.0, 0.0, 1.0}` - 背景色（真っ黒、不透明）
- `static constexpr Vec2 kDatePos{20, GameConst::kCommonUIHeight/2}` - 日付表示位置（左上、垂直中央）
- `static constexpr Vec2 kRemainingPos{300, GameConst::kCommonUIHeight/2}` - 残り日数表示位置（中央寄り上部、垂直中央）
- `static constexpr Vec2 kMentalPos = Vec2(GameConst::kWindowSize.x - 200, GameConst::kCommonUIHeight/2)` - 精神力表示位置（右上から200px左、垂直中央）
- `static constexpr ColorF kTextColor{1.0, 1.0, 1.0}` - テキスト色（白色）
- `static constexpr int32 kFontSize = 20` - フォントサイズ

## このクラスに含まれるpublicメソッド

- `static void Initialize()` - 初期化処理。現在は空実装（Scene幅/高さに依存する値はconstexprで直接設定済み）
- `static void Draw()` - 共通UIを描画。kBgRect.draw(kBgColor)で背景、FontManager::GetFont(U"ui_medium")でフォント取得、GameCommonData::GetCurrentDateString()で日付取得してkDatePosに描画、GameCommonData::GetRemainingDays()で残り日数取得してkRemainingPosに"残り日数: {}日"形式で描画、GameCommonData::GetMentalPower()で精神力取得してkMentalPosに"精神力: {}"形式で描画

## このクラスに含まれるprivateメソッド

なし

## このクラスで参照するアセットの情報

- フォント: `Asset/IBM_Plex_Sans_JP` - UI表示に使用

## このクラスが参照する仕様書の項目

- 共通UI
- 精神力

## このクラスが使用されるフェーズ

導入フェーズ、ゲームクリアフェーズ、ゲームオーバーフェーズ、バッドエンドフェーズ以外の全フェーズで表示される

## 特記事項・メモ

- モノステートパターンで実装し、静的メンバのみを使用する
- GameCommonDataの静的メソッドを呼び出してデータを取得する
- 背景は真っ黒（alpha=1.0）で画面上部全体を覆う
- 精神力は白色で統一表示（色分けなし）
- ゲーム開始時に`Initialize()`を必ず呼び出す必要がある
