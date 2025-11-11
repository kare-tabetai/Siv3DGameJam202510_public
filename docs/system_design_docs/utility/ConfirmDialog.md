# ConfirmDialog設計書

## 概要

「はい/いいえ」の単純な確認ダイアログを表示するユーティリティクラス。メッセージ表示、ボタン描画、入力判定を行う軽量なUIコンポーネントで、モーダル的に利用される。

## 目的・スコープ

このクラスは、ユーザーに対して簡単な確認（Yes/No）を求めるためのUIを提供する。描画と入力判定を行い、結果（はい/いいえ）を呼び出し元に伝える。ダイアログ以外の複雑な入力は対象外。

## ほかのクラスとの関係

- `FontManager` - メッセージやボタンのフォント取得に利用
- `UiConst` - 色や角丸、枠線太さなど共通UI定数を参照
- `ResumeUI` / `LoadingUI` 等のUIコンポーネント - 確認を求める場面で利用される

## このクラスが継承するクラス

なし

## このクラスのコンストラクタ

デフォルトコンストラクタを使用。特別な初期化は不要。

## このクラスのデストラクタ

デフォルトデストラクタを使用。

## このクラスに含まれるデータメンバ

- `bool isVisible_ = false` - ダイアログが表示されているかどうか（初期値false）
- `bool isConfirmed_ = false` - 「はい」が選ばれたかどうか（初期値false）
- `bool isCancelled_ = false` - 「いいえ」が選ばれたかどうか（初期値false）
- `String message_` - 表示するメッセージ（初期値空文字列）
- `Vec2 dialogPos_` - ダイアログの中心位置（初期値未定義）
- `bool showOnlyYes_ = false` - trueの場合は「はい」ボタンのみ表示（初期値false）
- `int32 messageFontSize_ = kMessageFontSize` - メッセージ描画用フォントサイズ（初期値20）

## このクラスに含まれる定数

- `static constexpr Vec2 kDefaultDialogSize{400,200}` - デフォルトダイアログサイズ
- `static constexpr Vec2 kDefaultButtonSize{100,40}` - ボタンサイズ
- `static constexpr double kCornerRadius = UiConst::kUiWindowRadius` - 角丸半径（UiConstから取得）
- `static constexpr double kButtonSpacing = 10.0` - ボタン間スペース
- `static constexpr ColorF kDialogBackgroundColor = UiConst::kWindowColor` - ダイアログ背景色
- `static constexpr ColorF kDialogBorderColor = UiConst::kWindowBorderColor` - ダイアログ枠線色
- `static constexpr ColorF kOverlayColor{0.0, 0.0, 0.0, 0.5}` - 半透明背景色（UiConstに定義なし）
- `static constexpr ColorF kTextColor = UiConst::kUITextColor` - テキスト色
- `static constexpr ColorF kYesButtonColor = UiConst::knormalButtonColor` - 「はい」ボタン通常色
- `static constexpr ColorF kYesButtonHoverColor = UiConst::khoverButtonColor` - 「はい」ボタンホバー色
- `static constexpr ColorF kNoButtonColor = UiConst::knormalButtonColor` - 「いいえ」ボタン通常色
- `static constexpr ColorF kNoButtonHoverColor = UiConst::khoverButtonColor` - 「いいえ」ボタンホバー色
- `static constexpr ColorF kButtonTextColor = Palette::White` - ボタンテキスト色
- `static constexpr int32 kMessageFontSize = 20` - メッセージフォントサイズ
- `static constexpr int32 kButtonFontSize = 18` - ボタンフォントサイズ

## このクラスに含まれるpublicメソッド

- `void Show(const String& message, const Vec2& dialogPos = Scene::Center(), bool showOnlyYes = false, int32 messageFontSize = kMessageFontSize)` - isVisible_=true、isConfirmed_=false、isCancelled_=false、message_=message、dialogPos_=dialogPos、showOnlyYes_=showOnlyYes、messageFontSize_=messageFontSizeで表示状態にする
- `void Hide()` - isVisible_=falseでダイアログを非表示にする
- `[[nodiscard]] bool IsVisible() const noexcept` - isVisible_を返す
- `[[nodiscard]] bool IsConfirmed() const noexcept` - isConfirmed_を返す
- `[[nodiscard]] bool IsCancelled() const noexcept` - isCancelled_を返す
- `void Update()` - isVisible_がfalseなら早期リターン。dialogRect計算、showOnlyYes_ならyesButtonを中央配置、それ以外は左右配置。yesButton.leftClicked()でisConfirmed_=true、isVisible_=false。showOnlyYes_がfalseでnoButton.leftClicked()ならisCancelled_=true、isVisible_=false
- `void Draw() const` - isVisible_がfalseなら早期リターン。Scene::Rect().draw(kOverlayColor)で半透明背景、dialogRect.rounded(kCornerRadius).draw(kDialogBackgroundColor).drawFrame(UiConst::kWindowBorderThickness, kDialogBorderColor)でダイアログ描画。font(message_).drawAt(messageFontSize_, font_pos, kTextColor)でメッセージ描画。yesButtonPos（showOnlyYes_で中央/通常で左配置）描画、ホバー判定でkYesButtonHoverColor/kYesButtonColor切り替え。showOnlyYes_がfalseならnoButton描画、ホバー判定でkNoButtonHoverColor/kNoButtonColor切り替え

## このクラスに含まれるprivateメソッド

（ヘッダにprivateメソッドは無し。内部処理はUpdate/Draw内に実装）

## このクラスで参照するアセットの情報

- フォント: `FontManager` のキー（例: `"ui_medium"`）を参照して描画

## このクラスが参照する仕様書の項目

- 求職活動フェーズ（ResumeUI の入力確認）
- 共通UI（モーダルダイアログ）

## イメージ図

無し

## このクラスが使用されるフェーズ

- 主に `JobSearchPhase`（履歴書の入力確定時）だが、汎用の確認ダイアログとして他のフェーズでも使用可能

## 特記事項・メモ

- `Show()` で表示状態にし、`Update()` 内でのボタンクリック判定で `isConfirmed_` / `isCancelled_` をセットして自動的に非表示にする
- showOnlyYes_パラメータで「はい」ボタンのみ表示するモードに切り替え可能（情報表示ダイアログに利用）
- showOnlyYes_がtrueの時はyesButtonを中央配置、falseの時は左右配置で「はい/いいえ」両方表示
- messageFontSize引数でメッセージのフォントサイズを動的に変更可能（デフォルトは20）
- 見た目・挙動は `UiConst` と `FontManager` に依存する
- 半透明オーバーレイ色kOverlayColorのみUiConstに定義がなく固定値{0.0, 0.0, 0.0, 0.5}を使用
