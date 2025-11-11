# PhaseType設計書

## 概要

ゲーム内のフェーズを列挙する `enum class PhaseType` と、その文字列変換ユーティリティを提供するヘルパー名前空間 `phase_type_util` の設計書。

## 目的・スコープ

フェーズ識別を列挙で扱いやすくし、デバッグやUI表示のために列挙値→文字列変換を一元化する。列挙の追加や順序変更はここで管理される。

## ほかのクラスとの関係

- `GameManager` / フェーズ遷移ロジック - フェーズ識別に使用
- UI（デバッグ表示や画面タイトル等） - フェーズ名表示に `phase_type_util::ToString` を使用

## このクラスが継承するクラス

無し（enum）

## このクラスのコンストラクタ

該当なし

## このクラスのデストラクタ

該当なし

## このクラスに含まれるデータメンバ

- 列挙値: `Introduction`, `Sunrise`, `JobSearch`, `SisterMessage`, `Work`, `Sunset`, `GameOver`, `BadEnd`, `GameClear`

## このクラスに含まれる定数

- `phase_type_util::Count` - 列挙の要素数
- `phase_type_util::Names` - 各列挙値に対応する `StringView` 配列

## このクラスに含まれるpublicメソッド

- `[[nodiscard]] inline StringView ToStringView(PhaseType p) noexcept` - 列挙値に対応する `StringView` を返す
- `[[nodiscard]] inline String ToString(PhaseType p)` - `StringView` を `String` に変換して返す

## このクラスで参照するアセットの情報

なし

## このクラスが参照する仕様書の項目

- フェーズ遷移図（game_spec.md）

## このクラスが使用されるフェーズ

全フェーズ（列挙で表現されるため）

## 特記事項・メモ

- 列挙を増やす/並び替えるときは `Count` と `Names` の整合性に注意
