// UTF-8 with BOM
#pragma once
#include <Siv3D.hpp>
#include <memory>

#include "FrameWork/LlamaCpp/LlamaModel.h"
#include "FrameWork/LlamaCpp/LlamaModelManager.h"
#include "FrameWork/Misc/LlmScoreCalculator.h"
#include "Game/llm_chat/LlmChatWindow.h"
#include "Game/utility/DebugUtil.h"

// LlmChatWindowを使用したテスト用UIクラス
class LlmChatTestUI {
public:
    LlmChatTestUI() = default;
    ~LlmChatTestUI() = default;

    // LlmChatTestUIの初期化
    // shared_model: LlamaModelManagerから取得した共有モデル
    // 戻り値: 初期化成功時はtrue、失敗時はfalse
    bool Initialize(std::shared_ptr<llama_cpp::LlamaModel> shared_model) {
        if (!shared_model) {
            DebugUtil::Console << U"LlmChatTestUI::Initialize: shared_model is null";
            return false;
        }

        // ChatMessageWindowの設定
        const Vec2 screen_size = Scene::Size();
        constexpr int font_size = 24;
        const Font test_font = { FontMethod::SDF, font_size };
        const Font fontEmoji{ font_size, Typeface::ColorEmoji };
        test_font.addFallback(fontEmoji);

        ChatMessageWindowDesc chat_window_desc;
        chat_window_desc.rect = RectF(50, 50, screen_size.x - 100, screen_size.y - 200);
        chat_window_desc.font = test_font;
        chat_window_desc.partner_name = U"妹ちゃん";
        chat_window_desc.background_color = Color{ 214, 211, 206 };
        chat_window_desc.text_color = Palette::Black;
        chat_window_desc.chat_item_background_color = Palette::White;
        chat_window_desc.header_background_color = Color{ 8, 36, 107 };
        chat_window_desc.padding = Vec2(15, 15);

        // LlmChatWindowの設定
        LlmChatWindowSetting chat_setting;
        chat_setting.input_area_height = 30.0;
        chat_setting.system_prompt = U" \
        あなたの名前は「かな」といいます。7歳の女の子です。\n \
        対話の相手はお兄ちゃんです。お兄ちゃんは落ち込んでいます。\n \
        あなたはお兄ちゃんの妹です。\n \
        あなたは50文字以内の日本語で返答します。\n ";

        // LlmChatWindowDescの作成
        LlmChatWindowDesc chat_desc;
        chat_desc.chat_window_desc = chat_window_desc;
        chat_desc.setting = chat_setting;

        // LlmChatWindowの作成
        chat_ui_ = std::make_unique<LlmChatWindow>(chat_desc);

        // LlamaTextGeneratorとシステムプロンプトの初期化
        if (!chat_ui_->Initialize(shared_model)) {
            DebugUtil::Console << U"Failed to initialize LlmChatWindow";
            return false;
        }

        DebugUtil::Console << U"LlamaTextGenerator initialized successfully";

        // LlmScoreCalculatorの初期化とテスト
        score_calculator_ = std::make_unique<LlmScoreCalculator>();
        if (!score_calculator_->Initialize(shared_model)) {
            DebugUtil::Console << U"Failed to initialize LlmScoreCalculator";
            return false;
        }

        return true;
    }

    // UIの更新処理
    void Update() {
        if (chat_ui_) {
            chat_ui_->Update();
        }
    }

private:
    std::unique_ptr<LlmChatWindow> chat_ui_;
    std::unique_ptr<LlmScoreCalculator> score_calculator_;
};
