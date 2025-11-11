// FontManager.h
#pragma once
#include <Siv3D.hpp>
#include <memory>

#include "Game/utility/DebugUtil.h"

// ゲーム内で使用する全てのフォントを一元管理するユーティリティクラス
// モノステートパターンで実装され、どこからでもアクセス可能
class FontManager {
  public:
  FontManager() = delete;
  ~FontManager() = delete;

  // 定数
  static constexpr int32 kDefaultFontSize = 24;  // デフォルトのフォントサイズ(ピクセル)
  static constexpr int32 kSmallFontSize = 18;    // 小さいフォントサイズ(ピクセル)
  static constexpr int32 kMediumFontSize = 24;   // 中サイズフォントサイズ(ピクセル)
  static constexpr int32 kLargeFontSize = 32;    // 大きいフォントサイズ(ピクセル)
  static inline const FilePath kIBMPlexSansPath = U"Asset/IBM_Plex_Sans_JP/IBMPlexSansJP-Regular.ttf";  // IBM Plex Sans JPフォントファイルパス
  static inline const FilePath kSourceHanSerifPath = U"Asset/source-han-serif-1.001R/SourceHanSerif-Medium.otf";  // Source Han Serifフォントファイルパス

  // publicメソッド
  // フォントの初期化処理。ゲーム起動時に一度だけ呼び出す
  static void Initialize() {
    if (isInitialized_) {
      assert(false && "FontManager: すでに初期化されています。");
      return;
    }

    // 絵文字用フォントの初期化
    // MEMO: カラー絵文字フォントをフォールバックフォントとして設定した場合、
    // 描画サイズは登録先のフォントのサイズに合わせらるためサイズは何でもいい
    emojiFont_ = std::make_unique<Font>(kDefaultFontSize, Typeface::ColorEmoji);

    // IBM Plex Sans JPフォントを各サイズで登録
    LoadFont(U"default", kIBMPlexSansPath, kDefaultFontSize);
    LoadFont(U"ui_small", kIBMPlexSansPath, kSmallFontSize);
    LoadFont(U"ui_medium", kIBMPlexSansPath, kMediumFontSize);
    LoadFont(U"ui_large", kIBMPlexSansPath, kLargeFontSize);

    // Source Han Serifフォントを各サイズで登録
    LoadFont(U"resume_small", kSourceHanSerifPath, kSmallFontSize);
    LoadFont(U"resume_medium", kSourceHanSerifPath, kMediumFontSize);
    LoadFont(U"resume_large", kSourceHanSerifPath, kLargeFontSize);

    isInitialized_ = true;
  }

  // 指定されたキー文字列に対応するフォントオブジェクトの参照を返す
  [[nodiscard]] static const Font& GetFont(const String& key) {
    if (!isInitialized_) {
      DebugUtil::Console << U"FontManager: 初期化されていません。";
      assert(false && "FontManager: 初期化されていません。");
    }

    if (!fontMap_.contains(key)) {
      DebugUtil::Console << U"FontManager: キー '" << key << U"' が見つかりません。デフォルトフォントを返します。";
      assert(false && "キー が見つかりません。");
    }

    return fontMap_[key];
  }

  // 指定されたキーのフォントが登録されているかを確認する
  [[nodiscard]] static bool HasFont(const String& key) noexcept {
    return fontMap_.contains(key);
  }

  private:
  // 指定されたパスからフォントを読み込み、キーと共にハッシュテーブルに登録する
  static void LoadFont(const String& key, const FilePath& path, int32 fontSize) {
    Font font{fontSize, path};

    if (!font) {
      DebugUtil::Console << U"FontManager: フォント '" << path << U"' の読み込みに失敗しました。デフォルトフォントを使用します。";
      font = Font{fontSize};
    }

    // 絵文字フォントをフォールバックとして追加
    assert(emojiFont_ && "FontManager: 絵文字フォントが初期化されていません。");
    if(emojiFont_){
      font.addFallback(*emojiFont_);
    }

    fontMap_[key] = font;
  }

  // データメンバ（モノステートパターンのため静的）
  static inline HashTable<String, Font> fontMap_;  // フォント名をキーとしてFontオブジェクトを保持するハッシュテーブル
  static inline bool isInitialized_ = false;       // 初期化済みフラグ
  // 絵文字用フォント（フォールバック用） MEMO:生のstaticだと、
  // 「E200 | エンジン起動前のアセット初期化」に引っかかるためスマポで初期化を遅延
  static inline std::unique_ptr<Font> emojiFont_;
};
