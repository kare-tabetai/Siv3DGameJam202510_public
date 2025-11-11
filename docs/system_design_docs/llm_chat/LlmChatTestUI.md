# LlmChatTestUI設計書

## 概要
LlmChatWindowを使用したテスト用UIクラス。LLM機能の動作確認に使用

## 目的・スコープ
- LlmChatWindowの動作テスト
- LlmScoreCalculatorの動作テスト
- 開発中のデバッグ・検証用

## ほかのクラスとの関係
- `LlmChatWindow` - テスト対象のチャットウィンドウ
- `LlamaModel` - LlamaModelManagerから取得した共有モデル(FrameWork)
- `LlamaModelManager` - LLMモデルの管理(FrameWork)
- `LlmScoreCalculator` - LLMを使ったスコア計算機能のテスト(FrameWork)

## このクラスが継承するクラス
無し

## このクラスのコンストラクタ
デフォルトコンストラクタ

## このクラスのデストラクタ
デフォルトデストラクタ

## このクラスに含まれるデータメンバ

- `std::unique_ptr<LlmChatWindow> chat_ui_` - テスト対象のチャットウィンドウUI（初期値nullptr）
- `std::unique_ptr<LlmScoreCalculator> score_calculator_` - スコア計算機能のテスト用（初期値nullptr）

## このクラスに含まれる定数
無し

## このクラスに含まれるpublicメソッド

- `bool Initialize(std::shared_ptr<llama_cpp::LlamaModel> shared_model)` - shared_modelがnullptrならConsoleへエラー出力してfalse返却。screen_size=Scene::Size()、font_size=24、test_font={FontMethod::SDF, font_size}でフォント作成、fontEmoji={font_size, Typeface::ColorEmoji}でtest_font.addFallback(fontEmoji)。chat_window_desc.rect=RectF(50, 50, screen_size.x-100, screen_size.y-200)、chat_window_desc.font=test_font、chat_window_desc.partner_name=U"妹ちゃん"、chat_window_desc.background_color=Color{214,211,206}、chat_window_desc.text_color=Palette::Black、chat_window_desc.chat_item_background_color=Palette::White、chat_window_desc.header_background_color=Color{8,36,107}、chat_window_desc.padding=Vec2(15,15)で設定。chat_setting.input_area_height=30.0、chat_setting.system_prompt=U"あなたの名前は「かな」といいます。7歳の女の子です。\n対話の相手はお兄ちゃんです。お兄ちゃんは落ち込んでいます。\nあなたはお兄ちゃんの妹です。\nあなたは50文字以内の日本語で返答します。\n"で設定。chat_desc{chat_window_desc, chat_setting}作成、chat_ui_=std::make_unique<LlmChatWindow>(chat_desc)で生成。chat_ui_->Initialize(shared_model)失敗時はConsoleへエラー出力してfalse返却。成功時はConsoleへ成功メッセージ出力。score_calculator_=std::make_unique<LlmScoreCalculator>()作成、score_calculator_->Initialize(shared_model)失敗時はConsoleへエラー出力してfalse返却。成功時はtrue返却
- `void Update()` - chat_ui_がnullptrでなければchat_ui_->Update()を呼び出し

## このクラスに含まれるprivateメソッド
無し

## このクラスで参照するアセットの情報
- フォント: `FontMethod::SDF`, サイズ24
- 絵文字フォント: `Typeface::ColorEmoji`, サイズ24

## このクラスが参照する仕様書の項目
- 妹メッセージUI(テスト用のため直接的な仕様参照は無し)

## イメージ図
無し(テスト用UI)

## このクラスが使用されるフェーズ
開発中のテスト専用

## 特記事項・メモ

- 本番のゲームでは使用されない開発用クラス
- システムプロンプトで「かな(7歳の女の子)」という妹キャラクターを設定
- システムプロンプトで50文字以内の返答を指示
- 画面サイズ(Scene::Size())に応じて動的にUIレイアウトを調整: rect=RectF(50, 50, screen_size.x-100, screen_size.y-200)
- 背景色: Color{214, 211, 206}, テキスト色: Palette::Black
- チャット項目背景色: Palette::White, ヘッダー背景色: Color{8, 36, 107}
- padding: Vec2(15, 15)
- フォントはSDF方式、サイズ24、絵文字フォールバックを追加
- partner_name: U"妹ちゃん"
- input_area_height: 30.0
- LlmChatWindowとLlmScoreCalculatorの両方を初期化してテスト可能
