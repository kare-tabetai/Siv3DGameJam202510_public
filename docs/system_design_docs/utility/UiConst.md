# UiConst設計書

## 概要

UIで共通利用する色・サイズ・アセットパスなどの定数をまとめた静的ユーティリティ。プロジェクト全体のUI表現を一元的に調整するための定義を提供する。

## 目的・スコープ

UI の見た目（色、角丸、枠線太さなど）を集中管理し、UI コンポーネント間で一貫した表現を担保する。具体的な描画や入力処理は対象外。

## ほかのクラスとの関係

- `ConfirmDialog`, `MessageWindowUI`, `ResumeUI`, `CommonUI` など全ての UI コンポーネントが参照する
- `FontManager` とは役割を分ける（FontManager はフォント管理、UiConst は色/形の定義）

## このクラスが継承するクラス

なし（struct で静的定数を持つ）

## このクラスのコンストラクタ

delete にしてインスタンス化不可

## このクラスのデストラクタ

delete にしてインスタンス化不可

## このクラスに含まれるデータメンバ

- `static constexpr ColorF kMonoEyeBlue` - 基本配色
- `static constexpr ColorF kWindowColor` - ウィンドウ背景色
- `static constexpr ColorF kWindowBorderColor` - ウィンドウ枠色
- `static constexpr ColorF kUITextColor` - UIテキスト色
- `static constexpr double kWindowBorderThickness` - ウィンドウ枠の太さ
- `static constexpr double kUiWindowRadius` - UI 角丸半径
- 入力エリア関連色/太さ/半径: `kInputAreaColor`, `kInputTextColor`, `kInputAreaBorderColor`, `kInputAreaThickness`, `kInputAreaRadius`
- ボタン色: `knormalButtonColor`, `khoverButtonColor`
- `static constexpr StringView kMonoEyeIconPath` - アイコンアセットパス

## このクラスに含まれる定数

上記の静的定数群が UI 表現を定義する

## このクラスに含まれるpublicメソッド

なし（定数のみ）

## このクラスで参照するアセットの情報

- `Asset/monoEye_icon_blue.png` - モノアイアイコン（`kMonoEyeIconPath`）

## このクラスが参照する仕様書の項目

- 共通UI、妹チャットの角丸表現／履歴書UIの角ばった表現など

## このクラスが使用されるフェーズ

全フェーズ（UI コンポーネントが存在する箇所）

## 特記事項・メモ

- 角丸や色味の変更はここを編集することでプロジェクト全体へ反映される
