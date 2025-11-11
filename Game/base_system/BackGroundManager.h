// BackGroundManager.h
#pragma once
#include <Siv3D.hpp>
#include <algorithm>

#include "Game/utility/DebugUtil.h"

// 背景画像を管理するモノステート（static）クラス
class BackGroundManager {
  private:
  // 背景テクスチャの識別子とアセットパスの定義
  static constexpr std::array<std::pair<StringView, StringView>, 2>
    s_backgroundAssets = {
      {
        {U"my_room", U"Asset/background.png"},
        {U"amusement", U"Asset/amusement_background.png"},
      }
    };

  // ロード済みの背景テクスチャ（識別子とTextureのペア）
  inline static Array<std::pair<String, Texture>> s_loadedTextures;

  // 現在選択されている背景の識別子
  inline static String s_currentBackgroundId = U"my_room";

  // 初期化済みフラグ
  inline static bool s_initialized = false;

  public:
  // 初期化処理（全テクスチャをロード）
  static void Initialize() {
    if (s_initialized) {
      return;
    }

    s_loadedTextures.clear();

    // 全ての背景テクスチャをロード
    for (const auto& [id, path] : s_backgroundAssets) {
      Texture texture(path);
      if (not texture) {
        // テクスチャのロードに失敗した場合はログ出力
        DebugUtil::Console << U"Failed to load background texture: " << path;
        continue;
      }
      s_loadedTextures.emplace_back(String(id), std::move(texture));
    }

    s_initialized = true;
    DebugUtil::Console << U"BackGroundManager initialized with " << s_loadedTextures.size()
          << U" textures.";

    SetBackGround(U"my_room");
  }

  // 背景を設定する（識別子で指定）
  static void SetBackGround(const String& backgroundId) {
    if (backgroundId.empty()) {
      s_currentBackgroundId = U"";
      return;
    }

    // 指定された識別子が存在するか確認
    bool found = false;
    for (const auto& [id, texture] : s_loadedTextures) {
      if (id == backgroundId) {
        found = true;
        break;
      }
    }

    if (found) {
      s_currentBackgroundId = backgroundId;
    } else {
      DebugUtil::Console << U"BackGround ID not found: " << backgroundId;
    }
  }

  // 現在の背景を描画する
  static void Draw() {
    if (s_currentBackgroundId.empty()) {
      return;
    }

    // 現在の背景テクスチャを検索して描画
    auto it = std::find_if(s_loadedTextures.begin(), s_loadedTextures.end(), [&](const auto& pair) { return pair.first == s_currentBackgroundId; });

    if (it != s_loadedTextures.end()) {
      // 画面全体に合わせて描画
      it->second.resized(Scene::Size()).draw();
    }
  }

  // 現在の背景IDを取得
  static const String& GetCurrentBackgroundId() {
    return s_currentBackgroundId;
  }

  // リセット処理（テクスチャを解放）
  static void Reset() {
    s_loadedTextures.clear();
    s_currentBackgroundId = U"my_room";
    s_initialized = false;
  }
};
