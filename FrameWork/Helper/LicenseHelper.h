#pragma once
#include <Siv3D.hpp>

// ライセンス表記を列挙する
namespace LicenseHelper {
void AddLicenses() {
  LicenseInfo info;

  // PhysX
  info.title = U"PhysX";
  info.copyright = U"Copyright (c) 2008-2024, NVIDIA Corporation";
  info.text = U"https://github.com/NVIDIA-Omniverse/PhysX/blob/main/LICENSE.md";
  LicenseManager::AddLicense(info);

  // flecs
  info.title = U"flecs";
  info.copyright = U"Copyright (c) 2019 Sander Mertens";
  info.text = U"https://github.com/SanderMertens/flecs";
  LicenseManager::AddLicense(info);

  // llama.cpp
  info.title = U"llama.cpp";
  info.copyright = U"Copyright (c) 2023-2024 The ggml authors";
  info.text = U"https://github.com/ggml-org/llama.cpp?tab=MIT-1-ov-file#readme";
  LicenseManager::AddLicense(info);

  // タイピング メカニカル単1
  // タイピング メカニカル単2
  // タイピング メカニカル単3
  // タイピング パンタグラフ単1
  info.title = U"Springin’ Sound Stock";
  info.text = U"https://www.springin.org/sound-stock/";
  LicenseManager::AddLicense(info);

  // wrench02
  // PC　マウス05
  // ホワイトノイズ
  // ショートアクセント01
  info.title = U"「OtoLogic」";
  info.text = U"https://otologic.jp/";
  LicenseManager::AddLicense(info);

  // Ｇ線上のアリア
  // 花のワルツ
  // ハンガリー舞曲
  // 月の光
  info.title = U"Classix";
  info.text = U"https://classix.sitefactory.info/index.html";
  LicenseManager::AddLicense(info);

  // 目アイコン6
  info.title = U"ICOOON MONO";
  info.copyright = U"© Copyright TopeconHeroes ! all right reserved.";
  info.text = U"https://icooon-mono.com/";
  LicenseManager::AddLicense(info);

  // gom_tex
  // checker_board_tex
  // metal_board_tex
  // amusement_background
  info.title = U"ChatGPT Sora";
  info.text = U"https://chatgpt.com/";
  LicenseManager::AddLicense(info);

  // metal_roung_tex.png
  info.title = U"Gemini Flash Image";
  info.text = U" https://gemini.google.com/";
  LicenseManager::AddLicense(info);

  // TODO:リソースを追加するたびにここに情報を追加
}
}  // namespace LicenseHelper
