# TweenUtil設計書

## 概要

ツウィーン（イージング）関数を提供する静的ユーティリティクラス。UIアニメーションの補間計算に使用する。

## 目的・スコープ

UI要素のアニメーション（フェード、移動、スケールなど）に必要なイージング関数を提供する。0.0～1.0の入力に対して、様々なカーブで補間された値を返す。個別のアニメーション管理は対象外。

## ほかのクラスとの関係

- `BlackOutUI` - フェードのイージングに使用する
- `MessageWindowUI` - 次メッセージマーカーのアニメーションに使用する
- 各UI系クラス - アニメーション効果に使用可能

## このクラスが継承するクラス

継承なし（静的クラス）

## このクラスのコンストラクタ

削除されている（静的クラスのため、インスタンス化不可）

## このクラスのデストラクタ

なし

## このクラスに含まれるデータメンバ

なし（静的メソッドのみ）

## このクラスに含まれる定数

なし

## このクラスに含まれるpublicメソッド

- `[[nodiscard]] static double Linear(double t) noexcept` - 線形補間（そのまま返す）。t ∈ [0, 1]
- `[[nodiscard]] static double EaseIn(double t) noexcept` - イーズイン（加速）。二次関数カーブ（t²）
- `[[nodiscard]] static double EaseOut(double t) noexcept` - イーズアウト（減速）。二次関数カーブ（1 - (1-t)²）
- `[[nodiscard]] static double EaseInOut(double t) noexcept` - イーズインアウト（加速→減速）。三次関数カーブ
- `[[nodiscard]] static double EaseInCubic(double t) noexcept` - イーズイン（三次）。より強い加速（t³）
- `[[nodiscard]] static double EaseOutCubic(double t) noexcept` - イーズアウト（三次）。より強い減速（1 - (1-t)³）
- `[[nodiscard]] static double EaseInOutCubic(double t) noexcept` - イーズインアウト（三次）。より滑らかな加減速
- `[[nodiscard]] static double EaseInQuart(double t) noexcept` - イーズイン（四次）。さらに強い加速（t⁴）
- `[[nodiscard]] static double EaseOutQuart(double t) noexcept` - イーズアウト（四次）。さらに強い減速（1 - (1-t)⁴）
- `[[nodiscard]] static double EaseInOutQuart(double t) noexcept` - イーズインアウト（四次）。最も滑らかな加減速
- `[[nodiscard]] static double Bounce(double t) noexcept` - バウンド効果。跳ねるような動き
- `[[nodiscard]] static double Elastic(double t) noexcept` - エラスティック効果。弾性のある動き

## このクラスに含まれるprivateメソッド

なし

## このクラスで参照するアセットの情報

なし

## このクラスが参照する仕様書の項目

なし（汎用ユーティリティ）

## このクラスが使用されるフェーズ

全フェーズで使用可能（UIアニメーションが必要な場合）

## 特記事項・メモ

- すべてのメソッドは静的メソッド（インスタンス化不要）
- すべてのメソッドは `[[nodiscard]]` と `noexcept` を付与
- 入力値 t は 0.0～1.0 の範囲を想定（0.0 = 開始、1.0 = 終了）
- 実装例（EaseInOut）：

  ```cpp
  static double EaseInOut(double t) noexcept {
      if (t < 0.5) {
          return 2.0 * t * t;
      } else {
          return 1.0 - 2.0 * (1.0 - t) * (1.0 - t);
      }
  }
  ```

- Siv3Dの標準イージング関数（`Easing::〜`）も利用可能だが、このクラスでプロジェクト独自の調整を行う
- BounceやElasticなどの高度なイージングは必要に応じて実装
- 使用例：

  ```cpp
  double alpha = TweenUtil::EaseInOut(fadeTimer_.sF() / fadeDuration_);
  ```
