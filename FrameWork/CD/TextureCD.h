#pragma once
#include <Siv3D.hpp>

struct TextureCD {
  TextureCD() = default;
  TextureCD(Texture&& t) : tex(t) {}

  Texture tex;
};
