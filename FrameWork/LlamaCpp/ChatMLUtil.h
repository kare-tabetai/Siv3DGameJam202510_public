#pragma once
#include <Siv3D.hpp>
#include <string>
#include <vector>

namespace llama_cpp {
/// @brief ChatMLメッセージの役割を表す列挙型
enum class ChatRole {
  System,    // システムメッセージ
  User,      // ユーザーメッセージ
  Assistant  // アシスタントメッセージ
};

/// @brief ChatMLメッセージ構造体
struct ChatMessage {
  ChatRole role;   // メッセージの役割
  String content;  // メッセージ内容

  /// @brief コンストラクタ
  ChatMessage(ChatRole r, const String& c) : role(r), content(c) {}
};

/// @brief ChatML形式のプロンプト変換ユーティリティクラス
class ChatMLUtil {
  public:
  /// @brief 会話履歴を含むChatML形式を生成する
  /// @param systemPrompt システムプロンプト
  /// @param conversation 会話履歴
  /// @param newUserMessage 新しいユーザーメッセージ
  /// @return ChatML形式の文字列
  static String CreateConversationChatML(const String& systemPrompt,
                                         const Array<ChatMessage>& conversation) {
    Array<ChatMessage> messages;

    // システムプロンプトを追加
    if (!systemPrompt.isEmpty()) {
      messages.emplace_back(ChatRole::System, systemPrompt);
    }

    // 会話履歴を追加
    for (const auto& message : conversation) {
      messages.emplace_back(message.role, message.content);
    }

    return ToChatML(messages);
  }

  private:
  /// @brief 単一のプロンプトをChatML形式に変換する
  /// @param prompt 変換するプロンプト文字列
  /// @param role メッセージの役割（デフォルトはUser）
  /// @return ChatML形式の文字列
  static String ToChatML(const String& prompt, ChatRole role = ChatRole::User) {
    return U"<|im_start|>" + GetRoleString(role) + U"\n" + prompt +
           U"\n<|im_end|>";
  }

  /// @brief 複数のメッセージをChatML形式に変換する
  /// @param messages メッセージのリスト
  /// @return ChatML形式の文字列
  static String ToChatML(const Array<ChatMessage>& messages) {
    String result;
    for (const auto& message : messages) {
      if (!result.isEmpty()) {
        result += U"\n";
      }
      result += ToChatML(message.content, message.role);
    }
    return result;
  }
  /// @brief ChatRoleを文字列に変換する内部関数
  /// @param role 変換するChatRole
  /// @return 役割文字列
  static String GetRoleString(ChatRole role) {
    switch (role) {
      case ChatRole::System:
        return U"system";
      case ChatRole::User:
        return U"user";
      case ChatRole::Assistant:
        return U"assistant";
      default:
        return U"user";
    }
  }

  // インスタンス化を禁止
  ChatMLUtil() = delete;
  ~ChatMLUtil() = delete;
  ChatMLUtil(const ChatMLUtil&) = delete;
  ChatMLUtil& operator=(const ChatMLUtil&) = delete;
};
}  // namespace llama_cpp
