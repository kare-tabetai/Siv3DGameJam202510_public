#pragma once
#include <Siv3D.hpp>

#include "ChatMessageItem.h"

// ChatMessageWindowの初期化パラメータを格納する構造体
struct ChatMessageWindowDesc {
  s3d::RectF rect;
  s3d::Font font;
  s3d::String partner_name;
  s3d::ColorF background_color = s3d::Palette::Black;
  s3d::ColorF text_color = s3d::Palette::White;
  s3d::ColorF chat_item_background_color = s3d::Palette::Gray;
  s3d::ColorF header_background_color = s3d::Palette::Darkgray;
  s3d::ColorF header_text_color = s3d::Palette::White;
  s3d::Vec2 padding = s3d::Vec2(10, 10);
};

enum class Sender {
  Self,
  Partner,
};

struct ChatMessageInfo {
  ChatMessageItem item;
  double time;  // メッセージが追加された時間
  Sender sender;
};

class ChatMessageWindow {
  public:
  // ChatItemの幅をウィンドウ幅の85%に設定
  static constexpr double kChatItemRate = 0.75;
  // メッセージ間の余白
  static constexpr double kItemMargin = 15.0;
  // ウィンドウ背景の角丸半径
  static constexpr double kWindowCornerRadius = 0.0;
  // ヘッダー部分の高さ
  static constexpr double kHeaderHeight = 40.0;

  // デフォルトコンストラクタを削除
  ChatMessageWindow() = delete;

  // パラメータ化されたコンストラクタ
  explicit ChatMessageWindow(const ChatMessageWindowDesc& desc)
      : m_desc(desc) {}

  // 位置を設定する関数
  void SetPosition(const s3d::Vec2& position) {
    s3d::Vec2 size = m_desc.rect.size;
    m_desc.rect = s3d::RectF(position, size);
    UpdateMessagePositions();  // 位置変更後にメッセージの座標を更新
  }

  // 位置を相対移動する関数
  void Move(const s3d::Vec2& offset) {
    m_desc.rect.moveBy(offset);
    UpdateMessagePositions();  // 移動後にメッセージの座標を更新
  }

  // ウィンドウの矩形を取得
  const s3d::RectF& GetRect() const { return m_desc.rect; }

  void AddMessage(const s3d::String& text, Sender sender) {
    auto item = CreateChatMessageItem(text, sender);

    // メッセージがウィンドウからはみ出る場合、古いメッセージを削除
    while (!GetRect().contains(item.GetRect())) {
      if (m_messages.empty()) {
        break;
      }
      m_messages.erase(m_messages.begin());
      UpdateMessagePositions();
      item = CreateChatMessageItem(text, sender);
    }

    ChatMessageInfo info{item, Scene::Time(), sender};
    m_messages.emplace_back(info);
  }

  void Draw() const {
    // ウィンドウの背景を角丸で描画
    m_desc.rect.rounded(kWindowCornerRadius).draw(m_desc.background_color);

    // ヘッダー部分を描画
    drawHeader();

    // メッセージを描画
    for (const auto& msg : m_messages) {
      msg.item.Draw();
    }
  }

  // すべてのメッセージをクリアする
  void Clear() {
    m_messages.clear();
  }

  private:
  // m_messagesの座標を現在の状態で更新するメソッド
  void UpdateMessagePositions() {
    double y = m_desc.rect.topY() + kHeaderHeight +
               m_desc.padding.y;  // ヘッダー高さを考慮した最初のメッセージのY位置

    for (auto& msg : m_messages) {
      s3d::Vec2 newPos;
      if (msg.sender == Sender::Self) {
        newPos = s3d::Vec2(m_desc.rect.rightX() - m_desc.padding.x, y);  // 右揃え
      } else {
        newPos = s3d::Vec2(m_desc.rect.leftX() + m_desc.padding.x, y);  // 左揃え
      }

      msg.item.SetPos(newPos);
      y += msg.item.GetRect().h + kItemMargin;  // メッセージの高さと余白を加算
    }
  }

  // ChatMessageItemを作成するstaticメソッド
  ChatMessageItem CreateChatMessageItem(const s3d::String& text,
                                        Sender sender) {
    MessageAreaStyle style;
    style.rect_color = m_desc.chat_item_background_color;
    style.text_color = m_desc.text_color;
    style.font = m_desc.font;
    style.corner_radius = 5.0;

    s3d::Vec2 pos;
    ChatMessageItem::Alignment alignment;
    if (sender == Sender::Self) {
      alignment = ChatMessageItem::Alignment::Right;
      pos = s3d::Vec2(m_desc.rect.rightX() - m_desc.padding.x,
                      calculateMessageY());  // 右揃え
    } else {
      alignment = ChatMessageItem::Alignment::Left;
      pos = s3d::Vec2(m_desc.rect.leftX() + m_desc.padding.x,
                      calculateMessageY());  // 左揃え
    }
    return ChatMessageItem(pos, m_desc.rect.w * kChatItemRate, text, style,alignment);  // 幅はウィンドウ幅の85%に設定
  }

  // ヘッダー部分を描画する関数
  void drawHeader() const {
    // ヘッダー領域
    s3d::RectF headerRect(m_desc.rect.x, m_desc.rect.y, m_desc.rect.w, kHeaderHeight);

    // ヘッダー背景を角丸で描画（上部のみ角丸）
    s3d::RoundRect(headerRect.x, headerRect.y, headerRect.w, headerRect.h,
                   kWindowCornerRadius)
      .draw(m_desc.header_background_color);

    // 相手の名前をヘッダー中央に描画
    s3d::Vec2 textPos(headerRect.center().x, headerRect.center().y);
    m_desc.font(m_desc.partner_name).drawAt(textPos, m_desc.header_text_color);
  }

  double calculateMessageY() {
    double y = m_desc.rect.topY() + kHeaderHeight +
               m_desc.padding.y;  // ヘッダー高さを考慮した最初のメッセージのY位置

    for (const auto& msg : m_messages) {
      y += msg.item.GetRect().h + kItemMargin;  // メッセージの高さと余白を加算
    }
    return y;
  }

  std::vector<ChatMessageInfo> m_messages;
  ChatMessageWindowDesc m_desc;  // ウィンドウの設定情報
};
