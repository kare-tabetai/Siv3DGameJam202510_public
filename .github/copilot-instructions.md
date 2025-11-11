# copilotへの基本指示

## 目的

このプロジェクトはSiv3Dを使用したC++23以降の環境で、docs/game_spec.mdに記載されたゲーム仕様のゲームを実装することを目的としています。

## 環境情報

- C++23以降の環境を想定(/std:c++latest)
- Siv3D v0.6.16
- Visual Studio 2022
- VSCode
- GitHub Copilot
- git

## プロジェクト構造

```text
Siv3DGameJam202510/
├── Main.cpp                              // アプリケーションのエントリーポイント
├── Siv3DFrameWork.sln                    // Visual Studioソリューションファイル
├── Siv3DFrameWork.vcxproj                // 標準版プロジェクトファイル
├── Siv3DFrameWork.vcxproj.filters        // 標準版プロジェクトフィルター定義
├── Siv3DFrameWorkNoCuda.vcxproj          // CUDA非依存版プロジェクトファイル
├── Siv3DFrameWorkNoCuda.vcxproj.filters  // CUDA非依存版プロジェクトフィルター定義
├── .github/
│   └── copilot-instructions.md           // GitHub Copilot向けプロジェクト指示書
├── App/                                  // 実行ファイル配置ディレクトリ(実行時のカレントディレクトリ)
│   ├── Asset/                            // ゲームアセット(画像、音声等)の配置場所
│   └── LlmModel/                         // ローカルLLMモデルファイルの配置場所
├── assistant_instruction_doc/            // 開発補助ドキュメント
│   ├── cppguide.html                     // Google C++スタイルガイド
│   └──Siv3D v0.6.16 における 3D 描画処理.md  // Siv3D 3D描画リファレンス
├── docs/                                 // プロジェクトドキュメント生成先
│   ├── game_spec.md                      // ゲーム仕様書
│   ├── architectural_design_doc.md       // アーキテクチャ設計書
│   ├── system_design_doc_template.md     // システム設計書テンプレート
│   └── system_design_docs/               // 生成されたクラス設計ドキュメント
├── FrameWork/                            // 汎用フレームワークコード
│   ├── flecs/                            // ECSライブラリ(サードパーティ、読み取り専用)
│   ├── llama_cpp/                        // LLMライブラリ(サードパーティ、読み取り専用)
│   └── PhysX/                            // 物理エンジン(サードパーティ、読み取り専用)
├── Game/                                 // ゲーム固有のコード配置先
├── .clang-format                         // コードフォーマット設定ファイル
├── .editorconfig                         // エディタ設定ファイル
└── prompts/                              // プロンプトテンプレート格納ディレクトリ
```

## 大局観(アーキテクチャ)

- サードパーティは読み取り専用: `FrameWork/flecs`, `FrameWork/llama_cpp`,  `FrameWork/PhysX`
- 本プロジェクトでは基本的にオブジェクト指向を活用する想定で、ECS(flecs)は物理関係の処理以外では使用しない。
- 汎用的に使えうる処理はFrameWork/以下に保存する
- このゲーム特有のコードはGame/以下に保存する
- C++コードはヘッダーオンリーで作成する。cppファイルは作成しない。
- マルチスレッド処理はLLM応答の場面以外では使用しない
- エラーハンドリングはPrint関数かConsole関数を使う。例外は使わない
- 60FPSの動作を想定

## ビルド/実行ワークフロー（VS Code）

- タスク: `Build Debug`（MSBuild で `Siv3DFrameWork.sln` を Debug|x64 ビルド）。
- 事後コピー: `Post Build - Copy Files` / `Post Build - Copy Files (NoCuda)` が EXE/DLL を所定の場所へコピー。
- 起動構成: `Run Siv3D App` と `Run Siv3D App (NoCuda)`。実行ファイルは `App/` 配下、作業ディレクトリは `Intermediate/.../Debug`。
  - NoCuda は CUDA/GPU 非依存の実行用構成（GPU のない PC でも動作させたい場合に使用）。
- コピー結果が「0 個」でも差分なしを示すだけで正常。実行確認は起動構成で行う。

## 配置と依存

- アセット/実行物: `App/`（`Asset/`, `engine/`, `dll/` など）。ビルド成果物は `Intermediate/` に生成。
- 外部: Siv3D, PhysX, llama.cpp（ローカル LLM は `LlmModel/Qwen3-4B-Instruct-2507-Q8_0.gguf` を使用）。
- インクルードは `.vscode/settings.json` で `SIV3D_0_6_16` などが通る前提。

## コーディング規約（要点）

- Google C++ Style 準拠。`#include` 順序: 1) ゲーム固有 2) フレームワーク 3) サードパーティ 4) 標準。
- コメントは必ず `//` の行コメント、日本語で簡潔に（`/* ... */`, `///` などは禁止）。このコメント規約は自前コード（`FrameWork/` 配下の自作ファイル）にのみ適用し、サードパーティ配下は対象外。
- ラムダのキャプチャは必要最小限を明示（例: `[&world, &selectedEntity]`）。必要なヘッダのみ `#include`。
- 想定外防止に `assert` を適宜使用。ドキュメントは必要時 `docs/` 配下へ生成。
- ヘッダーファイル（`.h`）作成時は必ず UTF-8 with BOM 形式で保存（C4819 警告回避）。
- .clang-formatファイルの内容も参照してコーディングをする

## ファイル、フォルダの運用

- `.h` を追加したら必ず対象の `*.vcxproj` と `*.vcxproj.filters` を更新し、フォルダ構造を反映。
- サードパーティ配下は編集しない。
- `assistant_instruction_doc/` 配下のファイルは参照専用ドキュメントであり、移動や削除は禁止。
- 外部のアセット素材やライブラリ以外の自作のフォルダ、ファイル名には全角文字を使わない
- クラス、構造体を定義したヘッダファイルは指定のない限り、「そのクラス、構造体名」.hとする
- ヘッダファイル以外のファイルとフォルダはスネークケースで名前を付ける

## ビルド構成の選択

- **標準版 (Siv3DFrameWork.vcxproj)**: CUDA対応GPU搭載PCでの開発・実行
- **NoCuda版 (Siv3DFrameWorkNoCuda.vcxproj)**: GPU非搭載PC、CUDA非対応GPUでの動作確認用
- 両方のビルドを配布する想定

## トラブルシュート（ビルド時のよくある項目）

- 文字コード警告（C4819）は該当ファイルを UTF-8 with BOM 保存で回避。
- 未使用引数は `[[maybe_unused]]` または `(void)arg;`。
- flecs/PhysX 由来の警告（到達不能など）は既知。エラー 0 であれば継続可。

## 仕様書、設計書

- ヘッダーファイルはdoces/system_design_docs/以下にあるクラス設計書から生成します
- クラス設計書はdoces/system_design_doc_template.mdの形式に合わせて生成されます
- docs/architectural_design_doc.mdは各クラス設計書を俯瞰したアーキテクチャ、関連性を示す設計書です
- docs/game_spec.mdをもとにして、architectural_design_doc.md、system_design_docs/以下にあるクラス設計書は作られました
- game_spec.md、architectural_design_doc.md、system_design_docs/以下にあるクラス設計書は互いに関連しあうものです。いづれかが変更された際は他のドキュメントに変更の必要が無いかをチェックしてください

## 参考

- Siv3D: [https://siv3d.github.io/ja-jp/](https://siv3d.github.io/ja-jp/) / サンプル: [https://siv3d.github.io/ja-jp/samples/3d/](https://siv3d.github.io/ja-jp/samples/3d/)
- Google C++ Style: [https://google.github.io/styleguide/cppguide.html](https://google.github.io/styleguide/cppguide.html)
