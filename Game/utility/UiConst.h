// UiConst.h
#pragma once
#include <Siv3D.hpp>

// UIで共通利用する定数をまとめた静的クラス
// 各値はプロジェクト内で参照しやすいように static  const として定義する
struct UiConst {
  UiConst() = delete;  // インスタンス化禁止

  // モノアイブルーの色定義
  static constexpr ColorF kMonoEyeBlue = {0.152, 0.298,0.411};

  // ウィンドウ（ダイアログ等）の背景色
  static constexpr ColorF kWindowColor = {0.84, 0.84, 0.86};

  // ウィンドウの枠線色
  static constexpr ColorF kWindowBorderColor = {0.8, 0.85, 0.9};

  // UIで使うテキストの色
  static constexpr ColorF kUITextColor = kMonoEyeBlue;

  // ウィンドウ枠の太さ
  static constexpr double kWindowBorderThickness = 4.0;

  // UIウィンドウの角丸半径
  static constexpr double kUiWindowRadius = 0.0;

  // 入力エリア（テキストボックス等）の背景色
  static constexpr ColorF kInputAreaColor{1.0, 1.0, 1.0};

  // 入力エリア（テキストボックス等）のテキスト色
  static constexpr ColorF kInputTextColor = {0.92, 0.92, 0.92};

  // 入力エリア（テキストボックス等）の枠線色
  static constexpr ColorF kInputAreaBorderColor = {0.3, 0.4, 0.5};

  // 入力エリア（テキストボックス等）の枠線の太さ
  static constexpr double kInputAreaThickness = 3.0;

  // 入力エリアの角丸半径
  static constexpr double kInputAreaRadius = 0.0;

  // 通常のボタン色
  static constexpr ColorF knormalButtonColor = {0.415, 0.521, 0.537};

  // ホバー時のボタン色
  static constexpr ColorF khoverButtonColor = kMonoEyeBlue;

  // モノアイアイコンのアセット
  static constexpr StringView kMonoEyeIconPath = U"Asset/monoEye_icon_blue.png";

};
