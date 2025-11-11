# ServerLoadingUI設計書

## 概要
国民統合情報サーバーとの通信中を示すローディングダイアログUIクラス。回転するアイコンとメッセージを表示する。

## 目的・スコープ
求職活動フェーズのパスワード入力後、サーバーから情報を取得する演出中に表示するローディングダイアログを管理する。`RotatingIcon`を利用して回転アイコンを表示し、メッセージを中央に配置する。

## ほかのクラスとの関係

- `RotatingIcon` - 回転するローディングアイコンの表示に使用
- `JobSearchPhase` - このクラスを使用してサーバー通信中の演出を表示
- `ResumeUI` - オートコンプリート中にこのUIを表示
- `FontManager` - フォント取得に使用
- `UiConst` - UI共通の定数値を参照

## このクラスが継承するクラス
なし

## このクラスのコンストラクタ

- `ServerLoadingUI()` - デフォルトコンストラクタ。`RotatingIcon`を初期化する

## このクラスのデストラクタ
デフォルトデストラクタ

## このクラスに含まれるデータメンバ

- `bool isVisible_ = false` - UIが表示中かどうかのフラグ
- `String message_` - ウィンドウに表示するメッセージ
- `RotatingIcon rotatingIcon_` - 回転アイコンのインスタンス

## このクラスに含まれる定数

- `static constexpr Vec2 kWindowCenterPos = GameConst::kWindowSize / 2` - ウィンドウ中心位置
- `static constexpr Vec2 kWindowSize{700, 300}` - ウィンドウサイズ
- `static constexpr Vec2 kTextOffset{0, -20}` - メッセージのオフセット
- `static constexpr double kCornerRadius = UiConst::kUiWindowRadius` - 角丸半径
- `static constexpr ColorF kDialogBackgroundColor = UiConst::kWindowColor` - ダイアログの背景色
- `static constexpr ColorF kDialogBorderColor = UiConst::kWindowBorderColor` - ダイアログの枠線色
- `static constexpr ColorF kTextColor = UiConst::kUITextColor` - テキストの色
- `static constexpr double kLoadingIconLogo = 80.0` - ロゴ表示サイズ（ピクセル）
- `static constexpr Vec2 kLoadingIconLogoOffset{0, kWindowSize.y / 4}` - ウィンドウ中央からのロゴオフセット
- `static constexpr double kMessageFontSize = 20` - メッセージのフォントサイズ

## このクラスに含まれるpublicメソッド

- `void Show(const String& message)` - UIを表示し、メッセージを設定する。回転アイコンも表示開始する
- `void Hide()` - UIを非表示にする
- `[[nodiscard]] bool IsVisible() const noexcept` - UIが表示中かどうかを返す
- `void Update()` - 毎フレーム呼ばれる更新処理。回転アイコンを更新する
- `void Draw() const` - 毎フレーム呼ばれる描画処理。ダイアログ、メッセージ、回転アイコンを描画する

## このクラスに含まれるprivateメソッド
なし

## このクラスで参照するアセットの情報

- ローディングアイコン: `Asset/loading_icon.png`（`RotatingIcon`経由で参照）

## このクラスが参照する仕様書の項目

- 求職活動フェーズ
- サーバー通信演出

## イメージ図
無し

## このクラスが使用されるフェーズ

- 求職活動フェーズ

## 特記事項・メモ

- `RotatingIcon`を内包して回転アニメーションを実現
- メッセージは可変で、呼び出し側から指定可能
- `UiConst`の共通定義を活用してUIの一貫性を保つ
