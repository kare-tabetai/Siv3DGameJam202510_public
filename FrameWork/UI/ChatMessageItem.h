#pragma once
#include <Siv3D.hpp>

#include "MessageAreaStyle.h"

class ChatMessageItem {
  public:
  enum class Alignment { Left, Right };

  ChatMessageItem() = default;

  ChatMessageItem(const s3d::Vec2& pos, double width, const s3d::String& text,
                  const MessageAreaStyle& style,
                  Alignment alignment = Alignment::Left)
      : m_pos(pos),
        m_width(width),
        m_text(text),
        m_style(style),
        m_alignment(alignment),
        m_rect(CalculateTextRect(pos, text, style, width, alignment)) {}

  void Draw() const {
    // 角丸矩形を描画
    m_rect.rounded(m_style.corner_radius).draw(m_style.rect_color);

    // テキストを矩形の範囲内に描画
    m_style.font(m_text).draw(m_rect, m_style.text_color);
  }

  // 矩形を取得するgetter
  const s3d::RectF& GetRect() const { return m_rect; }

  // 位置を設定するsetter
  void SetPos(const s3d::Vec2& pos) {
    m_pos = pos;
    m_rect = CalculateTextRect(m_pos, m_text, m_style, m_width, m_alignment);
  }

  static s3d::RectF CalculateTextRect(s3d::Vec2 pos, const s3d::String& text,
                                      const MessageAreaStyle& style,
                                      double width, Alignment alignment) {
    if (text.isEmpty()) {
      return s3d::RectF();
    }

    const double font_height = style.font.height();
    const double line_spacing = font_height * 0.2;
    const size_t max_lines = 100;  // 無限ループ防止

    for (size_t line_count = 1; line_count <= max_lines; line_count++) {
      const double height = font_height * line_count + line_spacing;

      RectF rec;
      if (alignment == Alignment::Left) {
        rec = RectF{Arg::topLeft = pos, width, height};
      } else {
        rec = RectF{Arg::topRight = pos, width, height};
      }

      if (style.font(text).fits(rec)) {
        return rec;
      }
    }
    
    // 最大行数に達した場合はそのサイズを返す
    const double max_height = font_height * max_lines + line_spacing;
    if (alignment == Alignment::Left) {
      return RectF{Arg::topLeft = pos, width, max_height};
    } else {
      return RectF{Arg::topRight = pos, width, max_height};
    }
  }

  private:
  s3d::Vec2 m_pos;
  double m_width;
  s3d::String m_text;
  MessageAreaStyle m_style;
  Alignment m_alignment;
  s3d::RectF m_rect;  // 事前に計算された矩形
};