// DebugUtil.h - デバッグ用ユーティリティ
// Siv3DのPrint/Console関数をラップし、統一的なデバッグ出力を提供する
// Releaseビルドでは出力を無効化する

#pragma once

#include <Siv3D.hpp>

namespace DebugUtil {

#ifdef _DEBUG
// Debugビルド時: 実際に出力を行う

// Consoleへの出力を行う関数のヘルパークラス
// s3d::Consoleと同様の << 演算子を提供
class ConsoleWriter {
 public:
  ConsoleWriter() = default;

  template <typename T>
  ConsoleWriter& operator<<(const T& value) {
    s3d::Console << value;
    return *this;
  }

  // マニピュレータ対応（例：std::endl）
  ConsoleWriter& operator<<(std::ostream& (*manip)(std::ostream&)) {
    // Siv3DのConsoleは標準のマニピュレータをサポートしないため何もしない
    return *this;
  }
};

// Print関数のラッパー
// 使用例: DebugUtil::Print(U"エラー: ", value);
template <typename... Args>
inline void Print(const Args&... args) {
  s3d::Print(args...);
}

#else
// Releaseビルド時: 出力を無効化する（何もしない）

// 何も出力しないダミーのConsoleWriterクラス
class ConsoleWriter {
 public:
  ConsoleWriter() = default;

  // すべての型を受け取るが何もしない
  template <typename T>
  ConsoleWriter& operator<<(const T& value) {
    (void)value;  // 未使用警告を抑制
    return *this;
  }

  // マニピュレータも無視
  ConsoleWriter& operator<<(std::ostream& (*manip)(std::ostream&)) {
    (void)manip;  // 未使用警告を抑制
    return *this;
  }
};

// Print関数のラッパー（何もしない）
template <typename... Args>
inline void Print(const Args&... args) {
  // 引数を使用済みとしてマークして警告を抑制
  ((void)args, ...);
}

#endif  // _DEBUG

// Console出力のラッパー
// 使用例: DebugUtil::Console << U"メッセージ" << value;
inline ConsoleWriter Console;

}  // namespace DebugUtil
