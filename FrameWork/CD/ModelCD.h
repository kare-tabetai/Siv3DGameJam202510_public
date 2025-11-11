#pragma once
#include <Siv3D.hpp>

struct ModelCD {
  ModelCD() = default;
  ModelCD(Model&& m, bool has_tex = false) : model(m) {
    if (has_tex) {
      if (!Model::RegisterDiffuseTextures(model, TextureDesc::MippedSRGB)) {
        s3d::Console << U"テクスチャ読み込み失敗";
      }
    }
  }

  Model model;
};
