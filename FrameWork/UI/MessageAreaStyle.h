#pragma once
#include <Siv3D.hpp>

// チャットメッセージの表示スタイルを定義する構造体
struct MessageAreaStyle {
  s3d::ColorF rect_color = s3d::Palette::Black;
  s3d::ColorF text_color = s3d::Palette::White;
  s3d::Font font;
  double corner_radius = 5.0;
};