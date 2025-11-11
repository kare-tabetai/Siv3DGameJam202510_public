// SisterMessageUI.h
#pragma once
#include <Siv3D.hpp>

#include "FrameWork/LlamaCpp/LlamaModel.h"
#include "FrameWork/UI/ChatMessageWindow.h"
#include "Game/llm_chat/LlmChatWindow.h"
#include "Game/utility/FontManager.h"
#include "Game/utility/GameConst.h"
#include "Game/utility/UiConst.h"

// TODO:妹の返信待ちの時間に既読や、入力中...の表示を追加する

// LlmChatWindowをラップし、妹とのLINE風チャットUIを提供するクラス
// SisterMessageUIManagerがshared_ptrで所有し、各フェーズはweak_ptrで参照する
class SisterMessageUI {
  public:
  static constexpr Vec2 kChatAreaSize = {GameConst::kPlayAreaSize.x * 2 / 3, GameConst::kPlayAreaSize.y * 8 / 10};
  static constexpr RectF kChatAreaRect = {GameConst::kPlayAreaCenter - kChatAreaSize / 2, kChatAreaSize};

  // 座布団（背景ウィンドウ）関連の定数
  // 横・縦で別々に余白を指定できるようにVec2にする
  static constexpr Vec2 kZabutonPad = Vec2{60.0, 40.0};  // チャット矩形より広げる余白 (x: 横, y: 縦)
  static constexpr double kZabutonCorner = 10.0;         // 角丸半径
  static constexpr double kZabutonFrameStretch = -2.0;   // 枠線描画のstretched量
  static constexpr double kZabutonFrameThickness = 2.0;  // 枠線の太さ

  // 座布団の描画色（ColorF型で保持）
  static constexpr ColorF kZabutonColor = ColorF(0.08, 0.08, 0.10);
  // 枠線色
  static constexpr ColorF kZabutonFrameColor = ColorF(0.8, 0.8, 0.8);

  // 位置・サイズ・矩形は Vec2 を使って計算して保持する
  static constexpr Vec2 kZabutonPos = kChatAreaRect.pos - kZabutonPad;
  static constexpr Vec2 kZabutonSize = kChatAreaRect.size + kZabutonPad * 2.0;
  // チャット矩形から計算される座布団矩形
  static constexpr RectF kZabutonRect = RectF{kZabutonPos, kZabutonSize};

  // 妹を演じさせるシステムプロンプト
  static constexpr s3d::StringView kSisterSystemPrompt =
    U"あなたの名前は「かな」といいます。7歳の女の子です。\n"
    U"対話の相手はお兄ちゃんです。お兄ちゃんは落ち込んでいます。\n"
    U"あなたはお兄ちゃんの妹です。\n"
    U"あなたは100文字以内の日本語で返答します。\n";

  // コンストラクタ: メンバ変数の初期化を行う
  SisterMessageUI() {
    auto& model_manager = llama_cpp::LlamaModelManager::GetInstance();
    auto shared_model = model_manager.GetModel(String(GameConst::kLlmModelId));

    assert(shared_model && "LlamaModelの取得に失敗しました。モデルが初期化されていることを確認してください。");
    if (shared_model) {
      Initialize(shared_model);
    }

    AddSisterMessage(U"お兄ちゃん元気?\nお仕事もらうの、うまくいった?", false);
  }

  // デストラクタ: デフォルトデストラクタを使用
  ~SisterMessageUI() = default;

  // LLMモデルを取得し、kSisterSystemPromptを使ってLlmChatWindowを初期化する
  // 成功時はtrue、失敗時はfalseを返す
  // SisterMessageUIManager::Initialize()の後に呼ばれる想定
  bool Initialize(std::shared_ptr<llama_cpp::LlamaModel> model) {
    // LlmChatWindowDescを構築
    ChatMessageWindowDesc chat_desc;
    chat_desc.rect = kChatAreaRect;
    // フォントは FontManager で一元管理されたものを使用する
    // 初期化はアプリ起動時に行われている想定
    chat_desc.font = FontManager::GetFont(U"ui_medium");
    chat_desc.partner_name = U"妹";
    // UI 共通定数を使うように変更
    chat_desc.background_color = UiConst::kWindowColor;
    chat_desc.text_color = UiConst::kInputTextColor;
    chat_desc.chat_item_background_color = UiConst::knormalButtonColor;
    chat_desc.header_background_color = UiConst::kMonoEyeBlue;
    chat_desc.header_text_color = UiConst::kInputTextColor;

    LlmChatWindowDesc llm_desc;
    llm_desc.chat_window_desc = chat_desc;
    llm_desc.setting.input_area_height = 40.0;
    llm_desc.setting.system_prompt = kSisterSystemPrompt;
    llm_desc.setting.max_input_char = 40;

    // LlmChatWindowを生成
    llmChatWindow_ = std::make_unique<LlmChatWindow>(llm_desc);

    // LlmChatWindowの初期化
    if (!llmChatWindow_->Initialize(model)) {
      return false;
    }

    return true;
  }

  // 毎フレーム呼ばれる更新処理
  // SisterMessageUIManager::Update()から呼ばれる
  void Update() {
    // MEMO: 更新処理はDrawで呼び出すため空
  }

  // 毎フレーム呼ばれる描画処理
  // 内部のLlmChatWindowの描画を行う
  // SisterMessageUIManager::Draw()から呼ばれる
  void Draw() {
    if (isActive_ && llmChatWindow_) {
      // 背景として "座布団" をチャット矩形より少し大きめに描画する
      // HACK: UpdateとDrawが一体化しているためここで描画順を制御する
      if (kChatAreaRect.w > 0 && kChatAreaRect.h > 0) {
        // クラス定数で定義した座布団矩形・色・パラメータを使う

        // 半透明の丸角矩形を描画
        RoundRect(kZabutonRect, kZabutonCorner).draw(kZabutonColor);

        // 軽い枠線を描画して浮き上がらせる
        RoundRect(kZabutonRect, kZabutonCorner).stretched(kZabutonFrameStretch).drawFrame(kZabutonFrameThickness, 0.0, kZabutonFrameColor);
      }

      // 内部ウィンドウの更新／描画を行う
      llmChatWindow_->Update();

      // チャット矩形の右端と座布団矩形の右端の中央に小さい円を描画
      const double circleX = (kChatAreaRect.rightX() + kZabutonRect.rightX()) / 2;
      const double circleY = kChatAreaRect.centerY();
      const double circleRadius = 18.0;
      Circle(circleX, circleY, circleRadius).drawFrame(2.0, ColorF(1.0, 1.0, 1.0, 1.0));

      // 円の中心に小さな枠だけの正方形を描画
      const double squareSize = 18.0;
      const RectF centerSquare(circleX - squareSize / 2, circleY - squareSize / 2, squareSize, squareSize);
      centerSquare.drawFrame(1.5, ColorF(1.0, 1.0, 1.0, 1.0));
    }
  }

  // ウィンドウのアクティブ状態を設定する
  void SetActive(bool active) noexcept {
    isActive_ = active;
    if (isActive_) {
      if (llmChatWindow_) {
        // ウィンドウをアクティブにしたときは入力エリアをクリアする
        llmChatWindow_->ClearInputArea();
        // メッセージ送信済みフラグをリセット
        isMessageSent_ = false;

        llmChatWindow_->SetInputAreaDisabled(false);

        llmChatWindow_->SetOnMessageSent(
          [this]([[maybe_unused]] s3d::StringView message) {
            // メッセージ送信時のコールバック
            isMessageSent_ = true;

            SoundManager::PlaySE(U"se_message");

            // 入力は一回までに制限するため、送信後は入力エリアを無効化する
            llmChatWindow_->SetInputAreaDisabled(true);
          });
      }
    }
  }

  void SetInputAreaDisabled(bool disabled) {
    if (llmChatWindow_) {
      llmChatWindow_->SetInputAreaDisabled(disabled);
    }
  }

  // ウィンドウがアクティブかどうかを取得する
  [[nodiscard]] bool IsActive() const noexcept {
    return isActive_;
  }

  // チャット履歴をクリアする
  // 内部的にLlmChatWindowのクリア処理を呼ぶ
  void Clear() {
    if (llmChatWindow_) {
      llmChatWindow_->Clear();
    }
  }

  // 妹のメッセージを直接チャット履歴に追加する
  // LLMを介さずにメッセージを表示したい場合に使用(誕生日メッセージやサービス終了通知など)
  // 内部的にLlmChatWindowのメッセージ追加処理を呼ぶ
  // 引数: message - 表示するメッセージ
  //       play_se - true の場合は既存動作通り通知音を鳴らす。false を渡すと通知音を鳴らさない（デフォルト: true）
  void AddSisterMessage(const s3d::String& message, bool play_se = true) {
    assert(llmChatWindow_ && "LlmChatWindowが初期化されていません。");
    if (llmChatWindow_) {
      // 通知音はオプション制に変更
      if (play_se) {
        SoundManager::PlaySE(U"se_message");
      }
      llmChatWindow_->AddMessage(message, Sender::Partner);
    }
  }

  // 妹からのメッセージを受け取り済みであればtrueを返す
  bool IsMessageReceived() const noexcept {
    // メッセージの応答待ちでは無くて、プレイヤーのメッセージは送信済みであればtrueを返す
    return !llmChatWindow_->IsWaitingForReply() && isMessageSent_;
  }

  private:
  // 内部で使用するチャットウィンドウ
  std::unique_ptr<LlmChatWindow> llmChatWindow_;

  // ウィンドウがアクティブ（表示）かどうかを示すフラグ
  bool isActive_ = false;

  // メッセージ送信済みフラグ プレイヤーがメッセージを送信済みならtrueになる
  bool isMessageSent_ = false;
};
