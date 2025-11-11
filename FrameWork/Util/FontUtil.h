#pragma once
#include <Siv3D.hpp>

#include "../SharedCD/FontSharedCD.h"
#include "flecs/flecs.h"

// フォント関連のユーティリティを提供する構造体
struct FontUtil {
  // コンストラクタを削除してインスタンス化を禁止
  FontUtil() = delete;

  static constexpr StringView StandardFontName = U"StandardFont";

  // 指定した名前のフォントCDエンティティを検索して返す
  // 引数: fontName - 検索するフォント名
  // 戻り値:
  // 該当するフォントCDのflecs::entity。見つからない場合は無効なentityを返す
  static flecs::entity FindFontCD(flecs::world& world,
                                  s3d::StringView fontName) {
    // フォントCDを全て走査して、nameが一致するentityを返す
    flecs::entity foundEntity;
    world.each<FontSharedCD>(
      [&foundEntity, fontName](flecs::entity e, const FontSharedCD& fontCD) {
        // nameが一致したらfoundEntityにセット
        if (fontCD.name == fontName) {
          foundEntity = e;
        }
      });
    // 見つかったentityを返す（見つからなければ無効なentity）
    return foundEntity;
  }

  // 標準フォント（StandardFont）のs3d::Fontを取得する
  // 戻り値:
  // 標準フォントのs3d::Font。見つからない場合はデフォルトフォントを返す
  static s3d::Font GetStandardFont(flecs::world& world) {
    // StandardFontという名前のフォントエンティティを検索
    flecs::entity standardFontEntity = FindFontCD(world, StandardFontName);

    // 見つかった場合はそのフォントを返す
    if (standardFontEntity.is_valid()) {
      const FontSharedCD* fontCD = standardFontEntity.try_get<FontSharedCD>();
      if (fontCD) {
        return fontCD->font;
      }
    }

    // 見つからない場合はデフォルトフォントを返す
    return s3d::Font();
  }
};
