# LlmChatWindow設計書

## 概要
LLM(大規模言語モデル)を使用したチャットウィンドウを管理するクラス。ユーザー入力を受け取り、LLMに送信し、応答を表示する

## 目的・スコープ
- LLMとのインタラクティブなチャット機能を提供
- ChatMessageWindowを使ったメッセージ履歴の表示
- テキスト入力エリアと送信ボタンの管理
- LLMの非同期応答生成の監視
- チャット完了時のコールバック通知機能

## ほかのクラスとの関係
- `ChatMessageWindow` - メッセージ履歴の描画と管理を担当
- `LlamaTextGenerator` - LLMテキスト生成を担当(FrameWork)
- `LlamaTextBuffer` - 生成中のテキストをバッファリング(FrameWork)
- `LlamaModel` - LLMモデル本体の管理(FrameWork)

## このクラスが継承するクラス
無し

## このクラスのコンストラクタ
- `explicit LlmChatWindow(const LlmChatWindowDesc& desc)` - ChatMessageWindowとLLM設定を含む記述子を受け取って初期化

## このクラスのデストラクタ
デフォルトデストラクタ

## このクラスに含まれるデータメンバ

- `llama_cpp::LlamaTextBuffer m_chat_message_buffer` - 生成中テキストを蓄えるバッファ（初期値デフォルトコンストラクタ）
- `std::shared_ptr<llama_cpp::LlamaTextGenerator> m_chat_message_generator` - テキスト生成器（初期値nullptr、Initialize()で設定）
- `LlmChatWindowSetting m_setting` - 表示等の設定（コンストラクタで受け取り）
- `ChatMessageWindow m_chat_window` - メッセージ履歴表示用ウィンドウ（コンストラクタで受け取ったdescから初期化）
- `TextEditState m_input_area` - テキスト入力状態（初期値デフォルトコンストラクタ）
- `bool m_input_area_disabled = false` - 入力エリアの編集不可フラグ（初期値false）
- `std::function<void(s3d::StringView)> m_on_message_receive` - 生成完了時の外部通知コールバック（初期値nullptr）
- `std::function<void(s3d::StringView)> m_on_message_sent` - メッセージ送信時の外部通知コールバック（初期値nullptr）
- `bool m_is_waiting_response = false` - LLM応答を待っているかのフラグ（初期値false）

### 関連構造体

#### LlmChatWindowSetting
- `double input_area_height = 30.0` - 入力エリアの高さ（ピクセル）
- `s3d::StringView system_prompt` - LLMに渡すシステムプロンプト（省略可）
- `int max_input_char = 100` - 最大入力文字数

#### LlmChatWindowDesc
- `ChatMessageWindowDesc chat_window_desc` - 内部で使うChatMessageWindowの設定
- `LlmChatWindowSetting setting` - LLMや入力エリアに関する設定

## このクラスに含まれる定数
無し

## このクラスに含まれるpublicメソッド

- `bool Initialize(std::shared_ptr<llama_cpp::LlamaModel> model)` - m_chat_message_generator=initializeChatMessageGenerator(model, m_setting.system_prompt)でジェネレータ初期化。成功すればtrueを返す。失敗時はfalse
- `void Update()` - m_chat_window.Draw()で履歴描画。m_is_waiting_response && m_chat_message_buffer.IsGenerationComplete()で応答完了監視、完了時にonResponseComplete(generated_text)呼び出してm_is_waiting_response=false。input_area_rect計算、SimpleGUI::TextBox(m_input_area, ..., !m_input_area_disabled)で入力エリア描画、send_button_rectでSimpleGUI::Button(U"▶", ..., !m_input_area_disabled)描画、クリック時にstartResponse(request)呼び出し
- `void SetOnMessageReceived(std::function<void(s3d::StringView)> on_message_receive)` - m_on_message_receive=on_message_receiveでコールバック登録
- `void SetOnMessageSent(std::function<void(s3d::StringView)> on_message_sent)` - m_on_message_sent=on_message_sentでコールバック登録
- `void Clear()` - m_chat_window.Clear()とm_input_area.clear()でチャット履歴と入力欄をクリア
- `void ClearInputArea()` - m_input_area.clear()で入力欄のみクリア
- `void AddMessage(const s3d::String& text, Sender sender)` - m_chat_window.AddMessage(text, sender)でメッセージを直接追加（LLM非経由）
- `[[nodiscard]] bool IsWaitingForReply() const noexcept` - m_is_waiting_responseを返す
- `void SetInputAreaDisabled(bool disabled)` - m_input_area_disabled=disabledで入力エリアの編集可否を設定

## このクラスに含まれるprivateメソッド

- `static std::shared_ptr<llama_cpp::LlamaTextGenerator> initializeChatMessageGenerator(std::shared_ptr<llama_cpp::LlamaModel> model, s3d::StringView system_prompt)` - generator=std::make_shared<llama_cpp::LlamaTextGenerator>()作成。context_config{context_size=1024, batch_size=512, threads=8, threads_batch=8}、sampling_config{temperature=0.7f, top_k=40, top_p=0.9f}で設定。generator->InitializeWithModel(model, context_config, sampling_config, String{system_prompt})で初期化、失敗時にConsoleへエラー出力してnullptr返却、成功時にgenerator返却
- `void startResponse(const llama_cpp::LlmRequest& request)` - m_chat_message_generatorがnullptrなら早期リターン。m_chat_message_buffer.ClearBuffer()でバッファクリア、m_chat_message_buffer.StartGeneration(*m_chat_message_generator, request)で生成開始、m_chat_window.AddMessage(request.prompt, Sender::Self)でユーザーメッセージ追加。m_on_message_sentが設定されていればm_on_message_sent(request.prompt)呼び出し。m_input_area.clear()で入力クリア、m_is_waiting_response=trueで待機状態設定
- `void onResponseComplete(const s3d::String& text)` - m_on_message_receiveが設定されていればm_on_message_receive(text)呼び出し。m_chat_window.AddMessage(text, Sender::Partner)でLLM応答をチャットに追加

## このクラスで参照するアセットの情報
無し(フォント等はChatMessageWindowDescで外部から渡される)

## このクラスが参照する仕様書の項目
- 妹メッセージフェーズ
- 妹メッセージUI

## イメージ図
![妹メッセージUIのイメージ図](../../spec_image/sister_message_ui.png)

## このクラスが使用されるフェーズ
- 妹メッセージフェーズ

## 特記事項・メモ

- LLMの応答生成は非同期で行われるため、Update()で完了を監視（m_chat_message_buffer.IsGenerationComplete()で判定）
- Initialize()を呼ばずにUpdate()を呼んでも安全（生成器がnullptrの場合は何もしない）
- コンテキスト設定: context_size=1024、batch_size=512、threads=8、threads_batch=8で固定
- サンプリング設定: temperature=0.7f、top_k=40、top_p=0.9fで固定
- 入力エリアはSimpleGUI::TextBoxで実装、送信ボタンは入力エリアの右端に正方形で配置
- 送信ボタンのラベルは"▶"（右向き三角）
- 入力エリアの編集可否はm_input_area_disabledフラグとSimpleGUI::TextBoxの最後の引数（!m_input_area_disabled）で制御
- ClearInputArea()は入力欄のみをクリア、Clear()はチャット履歴と入力欄の両方をクリア
- SetOnMessageReceived()とSetOnMessageSent()で外部からコールバックを登録可能（応答受信時と送信時の処理を分離）
