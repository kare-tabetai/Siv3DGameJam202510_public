#pragma once
#include <Siv3D.hpp>
#include <vector>

struct TextureArrayCD {
  TextureArrayCD() = default;
  TextureArrayCD(const std::vector<Texture>& textures) : textures(textures) {}
  TextureArrayCD(std::vector<Texture>&& textures)
      : textures(std::move(textures)) {}

  std::vector<Texture> textures;
};
