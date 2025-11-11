# GameConst設計書

## 概要

ゲーム全体で使用する定数を集合的に保持する静的（ヘッダオンリー）ユーティリティ。画面サイズ、UI配置、フェーズ初期値、LLMモデルID など、定数的な参照先を一元化する。

## 目的・スコープ

定数を1箇所に集約することで、画面レイアウトやフェーズ設定、アセット識別子などをプロジェクト全体で一貫して参照できるようにする。挙動や状態は保持せず、あくまで読み取り専用の定数群に限定する。

## ほかのクラスとの関係

- `GameManager` / 各 `Phase` - 画面サイズやプレイエリア、初期フェーズなどを参照
- `WorkPhase` - `kWorkAreaWidth` を用いた画面外判定に使用
- `LlmUtil` - `kLlmModelId` を組み合わせてモデルパスを構築

## このクラスが継承するクラス

なし（structで静的定数のみを持つ）

## このクラスのコンストラクタ

delete にしてインスタンス化不可

## このクラスのデストラクタ

delete にしてインスタンス化不可

## このクラスに含まれるデータメンバ

- `static constexpr auto kWindowSize` - ウィンドウサイズ (Point)
- `static constexpr StringView kWindowTitle` - ウィンドウタイトル
- `static constexpr PhaseType kInitialPhase` - 初期フェーズ
- `static constexpr double kCommonUIHeight` - 共通UIの高さ
- `static constexpr RectF kPlayAreaRect` - プレイエリア矩形
- `static constexpr Vec2 kPlayAreaCenter` - プレイエリア中心
- `static constexpr Vec2 kPlayAreaSize` - プレイエリアサイズ
- `static constexpr StringView kLlmModelId` - ローカルLLMモデルID
- `static constexpr double kWorkAreaWidth` - アルバイトフェーズの幅

## このクラスに含まれる定数

上記のデータメンバが全て定数として提供される。値はヘッダ内で定義され、ビルド時に決定される。

## このクラスに含まれるpublicメソッド

なし（静的定数のみ）

## このクラスに含まれるprivateメソッド

なし

## このクラスで参照するアセットの情報

- モデルファイル名構築に利用: `LlmModel/{kLlmModelId}.gguf`（実体は App/LlmModel に配置）

## このクラスが参照する仕様書の項目

- プレイエリア定義（アルバイトフェーズ）
- LLMモデルの参照（求職活動／妹メッセージ）

## イメージ図

無し

## このクラスが使用されるフェーズ

- 全フェーズ（レイアウト・共通設定の参照に利用）

## 特記事項・メモ

- ヘッダオンリーであるため、変更時は依存箇所の再ビルドに注意
- 画面解像度や UI 配置のチューニングはこのファイルの定数を更新することで一括反映可能
