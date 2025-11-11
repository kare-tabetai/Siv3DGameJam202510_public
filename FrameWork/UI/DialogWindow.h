#pragma once
#include <Siv3D.hpp>

// ダイアログの設定情報
// 使用例:
//   DialogDesc desc;
//   desc.font = Font(28);                     // フォント設定
//   desc.dialogSize = Vec2(0.8, 0.4);         // 画面の80%x40%
//   desc.position = Vec2(0.1, 0.3);           // 左上から10%, 30%の位置
//   desc.enableSequentialDisplay = true;     // 順次表示有効
//   desc.characterPerSec = 0.05;              // 1文字0.05秒間隔
//   desc.fontSizeOverride = 32;               // フォントサイズを32ptに
struct DialogDesc {
    Font font = Font(24);                           // 使用するフォント
    Vec2 dialogSize = Vec2(0.75, 0.25);            // ダイアログサイズ（画面比率で指定）
    Vec2 position = Vec2(0.125, 0.75);             // 位置（画面比率で指定）
    ColorF backgroundColor = ColorF(0, 0, 0, 0.8); // 背景色（半透明の黒）
    ColorF frameColor = ColorF(1, 1, 1, 1);        // 枠線色（白色）
    ColorF textColor = ColorF(1, 1, 1, 1);         // 文字色（白色）
    double characterPerSec = 0.05;                  // 1文字表示にかかる秒数
    bool enableSequentialDisplay = false;          // 順次表示を有効にするか
    Vec2 textPadding = Vec2(10, 10);               // テキストのパディング
    Optional<double> fontSizeOverride = none;      // フォントサイズのオーバーライド（未設定時はフォント本来のサイズを使用）
};

// ダイアログウィンドウクラス
// 
// 基本的な使用方法:
// 1. DialogDescでダイアログの外観と動作を設定
//    - font: フォント設定
//    - dialogSize, position: サイズと位置（画面比率）
//    - enableSequentialDisplay: 順次表示の有効/無効
//    - characterPerSec: 順次表示時の文字表示間隔
//    - fontSizeOverride: フォントサイズのオーバーライド（Optional）
// 
// 2. DialogWindowを作成してテキストを設定
//    DialogDesc desc;
//    desc.enableSequentialDisplay = true;
//    DialogWindow dialog(U"表示したいテキスト", desc);
// 
// 3. 表示制御
//    dialog.SetActive(true);   // 表示開始
//    dialog.SetActive(false);  // 非表示
// 
// 4. メインループで描画
//    dialog.Draw();
// 
// 5. 動的な操作
//    dialog.SetText(U"新しいテキスト");           // テキスト変更
//    dialog.SetFontSizeOverride(36);              // フォントサイズ変更
//    dialog.RestartSequentialDisplay();          // 順次表示リスタート
class DialogWindow {
private:
    String m_text = U"";                    // 表示するテキスト
    DialogDesc m_desc;                      // ダイアログ設定
    bool m_isVisible = false;               // 表示フラグ
    Rect m_dialogRect = Rect(0, 0, 0, 0);   // 実際の描画領域（ピクセル単位）
    Stopwatch m_stopwatch;                 // 順次表示用のタイマー

    // 画面比率からピクセル単位の矩形を計算する静的メソッド
    static Rect calculateDialogRect(const Vec2& position, const Vec2& dialogSize) {
        const int32 screenWidth = Scene::Width();
        const int32 screenHeight = Scene::Height();
        
        const int32 dialogWidth = static_cast<int32>(screenWidth * dialogSize.x);
        const int32 dialogHeight = static_cast<int32>(screenHeight * dialogSize.y);
        
        const int32 dialogX = static_cast<int32>(screenWidth * position.x);
        const int32 dialogY = static_cast<int32>(screenHeight * position.y);
        
        return Rect(dialogX, dialogY, dialogWidth, dialogHeight);
    }

    // 描画領域を計算
    void calculateRect() {
        m_dialogRect = calculateDialogRect(m_desc.position, m_desc.dialogSize);
    }

    // 文字エフェクト（上からゆっくり降ってくる表現）
    static void textEffect(const Vec2& penPos, double scale, const Glyph& glyph, const ColorF& color, double t) {
        const double y = EaseInQuad(Saturate(1 - t / 0.3)) * -20.0;
        const double a = Min(t / 0.3, 1.0);
        glyph.texture.scaled(scale).draw(penPos + glyph.getOffset(scale) + Vec2{0, y}, ColorF{color, a});
    }

    // テキスト描画位置を計算する静的メソッド
    static Vec2 calculateTextPosition(const Rect& dialogRect, const Vec2& textPadding) {
        return Vec2(dialogRect.x + textPadding.x, dialogRect.y + textPadding.y);
    }

    // テキスト描画矩形を計算する静的メソッド
    static RectF calculateTextRect(const Rect& dialogRect, const Vec2& textPadding) {
        return RectF(
            dialogRect.x + textPadding.x,
            dialogRect.y + textPadding.y,
            dialogRect.w - textPadding.x * 2,
            dialogRect.h - textPadding.y * 2
        );
    }

    // 描画可能領域を計算する静的メソッド
    static RectF calculateDrawableArea(const Rect& dialogRect, const Vec2& textPadding) {
        return RectF(
            dialogRect.x + textPadding.x,
            dialogRect.y + textPadding.y,
            dialogRect.w - textPadding.x * 2,
            dialogRect.h - textPadding.y * 2
        );
    }

    // グリフが描画可能領域内にあるかチェックする静的メソッド
    static bool isGlyphInBounds(const Vec2& penPos, double scale, const Glyph& glyph, const Font& font, const RectF& drawableArea) {
        return penPos.x >= drawableArea.x && 
               penPos.y >= drawableArea.y &&
               penPos.x + glyph.xAdvance * scale <= drawableArea.x + drawableArea.w &&
               penPos.y + font.height() * scale <= drawableArea.y + drawableArea.h;
    }

    // 改行が必要かチェックする静的メソッド
    static bool needsLineBreak(const Vec2& penPos, double scale, const Glyph& glyph, const RectF& drawableArea) {
        return penPos.x + glyph.xAdvance * scale > drawableArea.x + drawableArea.w;
    }

    // 縦方向の境界を超えているかチェックする静的メソッド
    static bool exceedsVerticalBounds(const Vec2& penPos, double scale, const Font& font, const RectF& drawableArea) {
        return penPos.y + font.height() * scale > drawableArea.y + drawableArea.h;
    }

    // 実際に使用するフォントサイズを取得する静的メソッド
    static double getEffectiveFontSize(const Font& font, const Optional<double>& fontSizeOverride) {
        return fontSizeOverride.value_or(font.fontSize());
    }

    // Glyph とエフェクトの関数を組み合わせてテキストを描画
    void drawSequentialText(const Font& font, double fontSize, const String& text, const Vec2& pos, const ColorF& color, double t) const {
        const double scale = (fontSize / font.fontSize());
        Vec2 penPos = pos;
        const ScopedCustomShader2D shader{Font::GetPixelShader(font.method())};

        // 描画可能領域を計算
        const RectF drawableArea = calculateDrawableArea(m_dialogRect, m_desc.textPadding);

        for (auto&& [i, glyph] : Indexed(font.getGlyphs(text))) {
            if (glyph.codePoint == U'\n') {
                penPos.x = pos.x;
                penPos.y += (font.height() * scale);
                
                // 縦方向の境界チェック
                if (exceedsVerticalBounds(penPos, scale, font, drawableArea)) {
                    break; // 描画領域を超えた場合は描画を停止
                }
                continue;
            }

            const double targetTime = (i * m_desc.characterPerSec);

            if (t < targetTime) {
                break;
            }

            // 横方向の境界チェック（自動改行）
            if (needsLineBreak(penPos, scale, glyph, drawableArea)) {
                penPos.x = pos.x;
                penPos.y += (font.height() * scale);
                
                // 縦方向の境界チェック
                if (exceedsVerticalBounds(penPos, scale, font, drawableArea)) {
                    break; // 描画領域を超えた場合は描画を停止
                }
            }

            // 描画領域内の場合のみ描画
            if (isGlyphInBounds(penPos, scale, glyph, font, drawableArea)) {
                textEffect(penPos, scale, glyph, color, (t - targetTime));
            }

            penPos.x += (glyph.xAdvance * scale);
        }
    }

public:
    // コンストラクタ・デストラクタ
    DialogWindow() 
    {
        calculateRect();
        m_stopwatch.reset();
    }

    DialogWindow(const String& text, const DialogDesc& desc = DialogDesc())
        : m_text(text)
        , m_desc(desc)
    {
        calculateRect();
        m_stopwatch.reset();
    }

    ~DialogWindow() = default;

    // ダイアログの表示/非表示を制御
    void SetActive(bool active) {
        m_isVisible = active;
        if (m_isVisible) {
            calculateRect(); // 画面サイズが変わっている可能性があるため再計算
            if (m_desc.enableSequentialDisplay) {
                m_stopwatch.restart(); // 順次表示を有効にしてタイマーをリスタート
            }
        }
    }

    // ダイアログを描画（メインループで毎フレーム呼び出し）
    void Draw() const {
        if (!m_isVisible) return;
        
        // 背景描画（設定された背景色）
        m_dialogRect.draw(m_desc.backgroundColor);
        
        // 枠線描画（設定された枠線色）
        m_dialogRect.drawFrame(2.0, m_desc.frameColor);
        
        // テキスト描画（設定された文字色）
        if (!m_text.isEmpty() && m_desc.font) {
            const double effectiveFontSize = getEffectiveFontSize(m_desc.font, m_desc.fontSizeOverride);
            
            if (m_desc.enableSequentialDisplay) {
                // 順次表示モード
                const Vec2 textPos = calculateTextPosition(m_dialogRect, m_desc.textPadding);
                const double t = m_stopwatch.sF();
                drawSequentialText(m_desc.font, effectiveFontSize, m_text, textPos, m_desc.textColor, t);
            } else {
                // 通常表示モード
                const RectF textRect = calculateTextRect(m_dialogRect, m_desc.textPadding);
                if (m_desc.fontSizeOverride) {
                    // フォントサイズがオーバーライドされている場合
                    const double scale = effectiveFontSize / m_desc.font.fontSize();
                    const ScopedCustomShader2D shader{Font::GetPixelShader(m_desc.font.method())};
                    
                    // スケールを適用した描画
                    Vec2 penPos = textRect.pos;
                    for (const auto& glyph : m_desc.font.getGlyphs(m_text)) {
                        if (glyph.codePoint == U'\n') {
                            penPos.x = textRect.pos.x;
                            penPos.y += (m_desc.font.height() * scale);
                            continue;
                        }
                        
                        glyph.texture.scaled(scale).draw(penPos + glyph.getOffset(scale), m_desc.textColor);
                        penPos.x += (glyph.xAdvance * scale);
                    }
                } else {
                    // 通常のフォントサイズを使用
                    m_desc.font(m_text).draw(textRect, m_desc.textColor);
                }
            }
        }
    }

    // 取得メソッド
    bool IsVisible() const {
        return m_isVisible;
    }

    // 表示テキストを変更（順次表示時は自動的にリスタート）
    void SetText(const String& text) {
        m_text = text;
        if (m_desc.enableSequentialDisplay && m_isVisible) {
            m_stopwatch.restart(); // テキスト変更時にタイマーをリスタート
        }
    }

    // 順次表示を最初からやり直す（テキストアニメーションのリプレイ）
    void RestartSequentialDisplay() {
        if (m_desc.enableSequentialDisplay) {
            m_stopwatch.restart();
        }
    }

    // 設定の更新
    void SetDesc(const DialogDesc& desc) {
        m_desc = desc;
        calculateRect();
        if (m_desc.enableSequentialDisplay && m_isVisible) {
            m_stopwatch.restart();
        }
    }

    // フォントサイズを動的に変更（元のフォント設定を上書き）
    void SetFontSizeOverride(double fontSize) {
        m_desc.fontSizeOverride = fontSize;
    }

    // フォントサイズを元の設定に戻す
    void ClearFontSizeOverride() {
        m_desc.fontSizeOverride = none;
    }

    // 現在の有効なフォントサイズを取得
    double GetEffectiveFontSize() const {
        return getEffectiveFontSize(m_desc.font, m_desc.fontSizeOverride);
    }
};