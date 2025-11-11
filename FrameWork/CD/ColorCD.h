#pragma once
#include <Siv3D.hpp>

struct ColorCD {
  ColorCD() = default;
  ColorCD(const Color& _c) : c(_c) {}
  Color c = Palette::White;
};
