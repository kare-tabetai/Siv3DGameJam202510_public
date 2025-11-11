# クラス設計書からヘッダーファイルを生成する

## タスクの概要

- system_design_doc/
- system_design_doc/
- system_design_doc/

のクラス設計書（`docs/system_design_docs/`配下のMarkdownファイル）を読み込み、C++ヘッダーファイル(.h)を生成してください。

## タスクの実行順序

1. 指定されたクラス設計書を読み込む
2. クラス設計書の「ほかのクラスとの関係」から依存関係を特定する
3. 関連するクラスの設計書とヘッダーファイルを確認し、整合性をチェックする
   - 関連クラスのメソッドシグネチャが設計書と一致しているか
   - 使用する型や戻り値が矛盾していないか
   - インターフェースの整合性が取れているか
4. 必要に応じて関連するクラス設計書も読み込む
5. クラス設計書の内容をもとにヘッダーファイルを生成する
6. 生成したヘッダーファイルを適切なディレクトリに配置する
7. `Game/`フォルダ内に設計書と同じフォルダ階層を作成
8. 例: 設計書が`docs/system_design_docs/utility/TweenUtil.md`の場合、ヘッダーは`Game/utility/TweenUtil.h`に配置
9. `Main.cpp`に生成したヘッダーのインクルードを追記する
10. vcxproj と vcxproj.filters ファイルに新規ヘッダーファイルを追加する

## 関連クラスとの整合性チェック

クラス設計書の「ほかのクラスとの関係」セクションに記載されている関連クラスについて、以下を確認してください：

### 確認項目

1. **関連クラスの設計書を確認**
   - `docs/system_design_docs/`配下で関連クラスの設計書を検索
   - 設計書に記載されているメソッド、戻り値、引数を確認

2. **既存のヘッダーファイルを確認**
   - `FrameWork/`または`Game/`配下で関連クラスのヘッダーファイルを検索
   - 実装済みの場合、そのインターフェースを確認

3. **整合性のチェックポイント**
   - **メソッド名**: 関連クラスで呼び出すメソッド名が正しいか
   - **戻り値の型**: 関連クラスから受け取る型が一致しているか
   - **引数の型**: 関連クラスに渡す引数の型が一致しているか
   - **const修飾**: メソッドのconst修飾が適切か
   - **static修飾**: 静的メソッドとして呼び出す場合、staticかどうか

### 矛盾が見つかった場合

- 設計書とヘッダーファイルで矛盾がある場合は、**ヘッダーファイルを優先**
- 設計書の記載が古い可能性があるため、実装を確認
- 明らかな誤りがある場合は、コメントで指摘

### チェック例

設計書に以下のような記載がある場合：

```markdown
## ほかのクラスとの関係

- `BlackOutUI` - フェードのイージングに使用する
```

確認すべき内容：

1. `BlackOutUI`の設計書またはヘッダーファイルを確認
2. `TweenUtil`のどのメソッドを使用するか確認
3. メソッドのシグネチャ（引数、戻り値）が一致しているか確認

## クラス設計書の解釈ガイド

### コンストラクタ/デストラクタ

- 設計書に記載がない場合: 記述不要
- 「削除されている」と記載がある場合: `= delete` を明示
- 引数が必要な場合: 設計書に従って定義

### 継承

- 設計書の「このクラスが継承するクラス」欄に従う

### データメンバ・定数

- 設計書の「データメンバ」「定数」をそのまま実装
- データメンバは `private:` または `protected:`
- 定数は `static constexpr` で `public:` に配置

### メソッド

- `[[nodiscard]]`, `const`, `noexcept` などの修飾子を適切に付与
- **メソッドの実装は記述せず、以下のルールに従う**:
  - コンストラクタ/デストラクタ: 本体は空 `{}`
  - `void` 型メソッド: 本体に `// TODO: 実装` のみ記述
  - 戻り値のあるメソッド: 適切な仮の戻り値を返し、その前に `// TODO: 実装` を記述
    - 数値型: `0`, `0.0`, `0.0f` など
    - ポインタ型: `nullptr`
    - bool型: `false`
    - オブジェクト型: デフォルトコンストラクタ `{}`
    - Optional型: `none`

### 静的クラス

- コンストラクタを `= delete`
- すべてのメソッドを `static`

## メソッド実装の記述ルール

### 基本方針

- **処理内容は実装しない**
- **コンパイルエラー回避のため最小限の記述のみ**
- **`// TODO: 実装` コメントを配置**

## ヘッダーファイルの配置

### フォルダ階層

- `Game/`フォルダ内にsystem_design_docs内のクラス設計書と同じフォルダ階層を作成して配置
- 設計書のパス: `docs/system_design_docs/{フォルダ}/{クラス名}.md`
- ヘッダーのパス: `Game/{フォルダ}/{クラス名}.h`

### 配置例

| 設計書 | ヘッダーファイル |
|--------|------------------|
| `docs/system_design_docs/utility/TweenUtil.md` | `Game/utility/TweenUtil.h` |
| `docs/system_design_docs/base_system/PlayerStatus.md` | `Game/base_system/PlayerStatus.h` |
| `docs/system_design_docs/job_search_phase/JobSearchPhase.md` | `Game/job_search_phase/JobSearchPhase.h` |

## Main.cppへのインクルード追記

生成したヘッダーファイルを`Main.cpp`にインクルードしてください。

### 追記場所

`Main.cpp`の既存のインクルード文の後に追加：

```cpp
#include <Siv3D.hpp>

// 既存のインクルード
// ...

// 新規追加
#include "Game/utility/TweenUtil.h"
```

### 注意事項

- インクルードパスは相対パス（`"Game/..."`）を使用
- アルファベット順または機能ごとにグループ化して追加

## vcxprojファイルへの追加

ヘッダーファイルを作成した後、**必ず**以下の4つのファイルに新規ヘッダーを追加してください：

1. `Siv3DFrameWork.vcxproj`
2. `Siv3DFrameWork.vcxproj.filters`
3. `Siv3DFrameWorkNoCuda.vcxproj`
4. `Siv3DFrameWorkNoCuda.vcxproj.filters`

### 追加例

ヘッダー: `Game/utility/TweenUtil.h`

**vcxproj:**

```xml
<ClInclude Include="Game\utility\TweenUtil.h" />
```

**vcxproj.filters:**

```xml
<ClInclude Include="Game\utility\TweenUtil.h">
  <Filter>Game\utility</Filter>
</ClInclude>
```

## 完了チェックリスト

ヘッダーファイル生成後は以下を確認してください：

- [ ] UTF-8 with BOM 形式で保存されている
- [ ] `Game/`フォルダ内に設計書と同じフォルダ階層で配置されている
- [ ] クラス設計書の内容がすべて反映されている
- [ ] 関連クラスとの整合性がチェックされている
- [ ] `Main.cpp`にインクルードが追記されている
- [ ] 4つのvcxproj/filtersファイルすべてに追加されている
- [ ] ビルドが通ることを確認（`Build Debug` タスク実行）
- [ ] コンパイルエラーや警告がないことを確認
