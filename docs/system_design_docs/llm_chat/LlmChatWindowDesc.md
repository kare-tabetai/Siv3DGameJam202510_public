# LlmChatWindowDesc設計書

## 概要
LlmChatWindowの構築に必要なパラメータをまとめた記述子構造体

## 目的・スコープ
- LlmChatWindow初期化時に必要な全パラメータを一つの構造体にまとめる
- ChatMessageWindowの設定とLLM関連設定を統合

## ほかのクラスとの関係
- `LlmChatWindow` - この記述子を使ってコンストラクタで初期化される
- `ChatMessageWindowDesc` - チャットメッセージ表示部分の設定を含む
- `LlmChatWindowSetting` - LLMと入力エリアの設定を含む

## このクラスが継承するクラス
無し(構造体)

## このクラスのコンストラクタ
デフォルトコンストラクタ

## このクラスのデストラクタ
デフォルトデストラクタ

## このクラスに含まれるデータメンバ
- `ChatMessageWindowDesc chat_window_desc` - 内部で使うChatMessageWindowの設定
- `LlmChatWindowSetting setting` - LLMや入力エリアに関する設定

## このクラスに含まれる定数
無し

## このクラスに含まれるpublicメソッド
無し(構造体のため全メンバがpublic)

## このクラスに含まれるprivateメソッド
無し

## このクラスで参照するアセットの情報
無し

## このクラスが参照する仕様書の項目
- 妹メッセージUI

## イメージ図
無し

## このクラスが使用されるフェーズ
- 妹メッセージフェーズ

## 特記事項・メモ
- Descriptor(記述子)パターンを採用し、複雑な初期化パラメータを構造化
