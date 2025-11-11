# SisterMessageUI設計書

## 概要

`LlmChatWindow`をラップし、妹とのLINE風チャットUIを提供するインスタンスクラス。`SisterMessageUIManager` が `std::shared_ptr<SisterMessageUI>` で所有し、各フェーズは `std::weak_ptr<SisterMessageUI>` を保持して利用する設計を想定する。内部的に`LlmChatWindow`を使用してLLMとの対話機能を実現する。

## 目的・スコープ

- `LlmChatWindow`を妹キャラクター用に初期化・管理する
- `SisterMessageUIManager`から取得した初期化済みLLMモデルを使用する
- 妹を演じさせるシステムプロンプトを設定する
- `LlmChatWindow`のラッパーとして、妹チャットUIの表示/非表示を制御する
- LLMとの連携(入力の送信と返信の受信)は`LlmChatWindow`に委譲する

## ほかのクラスとの関係

- `SisterMessagePhase` - このクラスを使用してLINE風UIを表示・管理する
- `BadEndPhase` - このクラスを使用してバッドエンド時のUI表示を行う
- `LlmChatWindow` - 内部で使用するチャットウィンドウ本体
- `SisterMessageUIManager` - このクラスのインスタンスを所有・管理し、初期化済みLLMモデルの取得元でもある
- `LlamaModel` - LLMモデル本体(FrameWork)

## このクラスが継承するクラス

なし

## このクラスのコンストラクタ

- `SisterMessageUI()` - LlamaModelManager::GetInstance().GetModel(GameConst::kLlmModelId)でモデル取得、Initialize(shared_model)呼び出し、AddSisterMessage(U"お兄ちゃん元気?\nお仕事もらうの、うまくいった?", false)で初期メッセージ追加。`SisterMessageUIManager` 側で生成される想定。

## このクラスのデストラクタ

デフォルトデストラクタを使用

## このクラスに含まれるデータメンバ

- `std::unique_ptr<LlmChatWindow> llmChatWindow_` - 内部で使用するチャットウィンドウ
- `bool isActive_ = false` - ウィンドウがアクティブ（表示）かどうかを示すフラグ（初期値false）
- `bool isMessageSent_ = false` - メッセージ送信済みフラグ。プレイヤーがメッセージを送信済みならtrueになる（初期値false）

## このクラスに含まれる定数

- `static constexpr Vec2 kChatAreaSize = {GameConst::kPlayAreaSize.x * 2 / 3, GameConst::kPlayAreaSize.y * 8 / 10}` - チャットエリアのサイズ
- `static constexpr RectF kChatAreaRect = {GameConst::kPlayAreaCenter - kChatAreaSize / 2, kChatAreaSize}` - チャットエリアの矩形
- `static constexpr Vec2 kZabutonPad = Vec2{60.0, 40.0}` - 座布団（背景ウィンドウ）の余白（x:横、y:縦）
- `static constexpr double kZabutonCorner = 10.0` - 座布団の角丸半径
- `static constexpr double kZabutonFrameStretch = -2.0` - 座布団枠線描画のstretched量
- `static constexpr double kZabutonFrameThickness = 2.0` - 座布団枠線の太さ
- `static constexpr ColorF kZabutonColor = ColorF(0.08, 0.08, 0.10)` - 座布団の描画色
- `static constexpr ColorF kZabutonFrameColor = ColorF(0.8, 0.8, 0.8)` - 座布団枠線色
- `static constexpr Vec2 kZabutonPos = kChatAreaRect.pos - kZabutonPad` - 座布団の位置
- `static constexpr Vec2 kZabutonSize = kChatAreaRect.size + kZabutonPad * 2.0` - 座布団のサイズ
- `static constexpr RectF kZabutonRect = RectF{kZabutonPos, kZabutonSize}` - 座布団の矩形
- `static constexpr s3d::StringView kSisterSystemPrompt` - 妹を演じさせるシステムプロンプト（"あなたの名前は「かな」といいます。9歳の女の子です。..."、100文字以内の日本語で返答）

## このクラスに含まれるpublicメソッド

- `bool Initialize(std::shared_ptr<llama_cpp::LlamaModel> model)` - LLMモデルを受け取り、ChatMessageWindowDescとLlmChatWindowDescを構築してLlmChatWindow初期化。FontManager::GetFont(U"ui_medium")でフォント、UiConst定数で色設定。成功時は`true`、失敗時は`false`を返す
- `void Update()` - 毎フレーム呼ばれる更新処理。空実装（UpdateとDrawが一体化しているためDrawで処理）。`SisterMessageUIManager::Update()`から呼ばれる
- `void Draw()` - 毎フレーム呼ばれる描画処理。isActive_がtrueなら座布団（RoundRect(kZabutonRect, kZabutonCorner).draw(kZabutonColor)と枠線）描画、llmChatWindow_->Update()呼び出し、円と正方形のアイコン描画。`SisterMessageUIManager::Draw()`から呼ばれる
- `void SetActive(bool active) noexcept` - isActive_=activeで状態設定。activeがtrueならllmChatWindow_->ClearInputArea()、isMessageSent_=false、llmChatWindow_->SetInputAreaDisabled(false)、メッセージ送信コールバック（isMessageSent_=true、SoundManager::PlaySE(U"se_message")、llmChatWindow_->SetInputAreaDisabled(true)）を設定
- `void SetInputAreaDisabled(bool disabled)` - llmChatWindow_->SetInputAreaDisabled(disabled)で入力エリア無効化/有効化
- `[[nodiscard]] bool IsActive() const noexcept` - isActive_を返す
- `void Clear()` - llmChatWindow_->Clear()でチャット履歴をクリア
- `void AddSisterMessage(const s3d::String& message, bool play_se = true)` - play_seがtrueならSoundManager::PlaySE(U"se_message")、llmChatWindow_->AddMessage(message, Sender::Partner)で妹のメッセージを直接チャット履歴に追加
- `bool IsMessageReceived() const noexcept` - !llmChatWindow_->IsWaitingForReply() && isMessageSent_を返す。妹からのメッセージを受け取り済みであればtrueを返す

## このクラスに含まれるprivateメソッド

なし(全ての処理は`LlmChatWindow`に委譲)

## このクラスで参照するアセットの情報

- LlmChatWindow内で使用されるフォントやSEについては`LlmChatWindow.md`を参照

## このクラスが参照する仕様書の項目

- 妹メッセージフェーズ
- LINE風UI
- 妹について
- フェーズごとのUI

## イメージ図

![妹メッセージUIのイメージ図](../spec_image/sister_message_ui.png)

## このクラスが使用されるフェーズ

- 妹メッセージフェーズ
- バッドエンドフェーズ

## 特記事項・メモ

- このクラスは`LlmChatWindow`のラッパークラスとして機能する
- `SisterMessageUIManager`がこのクラスのインスタンスを所有・管理する
- 妹を演じさせるシステムプロンプトは定数`kSisterSystemPrompt`として定義される
- `LlmChatWindow`の具体的な動作仕様については`LlmChatWindow.md`を参照
- `isActive_` フラグによりウィンドウの表示/非表示を制御可能。フェーズによってUIの表示を切り替えたい場合に使用する
- チャット履歴のスクロール機能は実装しない(仕様書の「コストを考えてスクロールなどは実装しない」に準拠)
- UI詳細の実装は`LlmChatWindow`に委譲される
- `Update()`と`Draw()`は`SisterMessageUIManager`から呼ばれる

## 所有とライフサイクル

- `SisterMessageUI` のインスタンスは `SisterMessageUIManager` が生成/所有し、ゲーム全体で共有される。各フェーズ（例: `SisterMessagePhase`, `BadEndPhase`）は `SisterMessageUIManager::GetSisterMessageUI()` で取得した `std::weak_ptr<SisterMessageUI>` を保持し、必要時に `lock()` して UI にアクセスする。これにより、UI のライフサイクルを中央で管理しつつ、フェーズ側の参照が dangling しないように設計する。
