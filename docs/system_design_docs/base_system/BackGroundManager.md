# BackGroundManager 設計書

## 概要
背景画像の読み込み・保持と描画を行うモノステート（static）クラス。ゲーム全体で共有される背景テクスチャを管理し、フェーズから識別子で背景を切り替えられるようにする。

## 目的・スコープ

- 画面全体に表示する背景画像(Texture)の読み込み、保持、描画を担当する。
- 背景アセットのパスと識別子はクラス内部で定義し、識別子を指定して背景を切り替えるための public API を提供する。
- テクスチャのロード失敗時はログ出力するが、リトライや代替テクスチャ配備は行わない（呼び出し元でのフォールバックを想定）。

## ほかのクラスとの関係

- `GameManager` - 初期化時に `BackGroundManager::Initialize()` を呼び出す。
- `WorkPhase`, `SunrisePhase`, `SunsetPhase`, その他フェーズ - 必要に応じて `BackGroundManager::SetBackGround(...)` を呼び出す。

## このクラスが継承するクラス

- なし（static モノステート実装）

## このクラスのコンストラクタ

- デフォルトコンストラクタのみ（利用不可）

## このクラスのデストラクタ

- デフォルトデストラクタのみ（利用不可）

## このクラスに含まれるデータメンバ

- `inline static Array<std::pair<String, Texture>> s_loadedTextures` - ロード済みの背景テクスチャの配列。先頭要素が識別子、2要素目が対応する `Texture`（初期値空配列）
- `inline static String s_currentBackgroundId = U"my_room"` - 現在描画対象の背景識別子（初期値"my_room"）
- `inline static bool s_initialized = false` - 初期化済みフラグ（初期値false）

## このクラスに含まれる定数

- `static constexpr std::array<std::pair<StringView, StringView>, 2> s_backgroundAssets` - (識別子, アセットパス) の定義配列。要素数2: ("my_room", "Asset/background.png"), ("amusement", "Asset/amusement_background.png")

## このクラスに含まれるpublicメソッド

- `static void Initialize()` - s_initializedがtrueなら早期リターン。s_loadedTextures.clear()、s_backgroundAssetsの全要素をループしてTexture(path)でロード、失敗時はConsoleログ出力、成功時はs_loadedTextures.emplace_back(String(id), std::move(texture))。s_initialized=true、SetBackGround(U"my_room")で初期背景設定
- `static void SetBackGround(const String& backgroundId)` - backgroundId.empty()ならs_currentBackgroundId=U""で空設定。s_loadedTexturesをループして識別子検索、見つかればs_currentBackgroundId=backgroundId、見つからなければConsoleログ出力
- `static void Draw()` - s_currentBackgroundId.empty()なら早期リターン。std::find_ifでs_currentBackgroundIdを検索、見つかればit->second.resized(Scene::Size()).draw()で画面全体に描画
- `static const String& GetCurrentBackgroundId()` - s_currentBackgroundIdを返す
- `static void Reset()` - s_loadedTextures.clear()、s_currentBackgroundId=U"my_room"、s_initialized=falseでリセット

## このクラスに含まれるprivateメソッド

- （なし。全て public static / static データメンバで実装されている）

## このクラスで参照するアセットの情報

- 背景画像: `Asset/background.png`（識別子: `my_room`）

## このクラスが参照する仕様書の項目

- 各フェーズ（`SunrisePhase`, `WorkPhase`, `SunsetPhase` 等）での背景表示

## イメージ図

- 無し（簡単な背景描画のため図は不要）

## このクラスが使用されるフェーズ

- `SunrisePhase`
- `WorkPhase`
- `SunsetPhase`
- その他、背景切り替えが必要なフェーズ

## 特記事項・メモ

- モノステート（static）実装のためスレッド安全性は想定していない。LLM 等のマルチスレッド処理とは分離して利用すること。
- リソース管理はシンプルに `Array::clear()` を行っている。必要なら将来 `Texture` のフォールバックや遅延ロードに拡張可能。
# BackGroundManager 設計書

## 概要
背景画像の読み込み・保持と描画を行うモノステート（static）クラス。ゲーム全体で共有される背景テクスチャを管理し、フェーズから識別子で背景を切り替えられるようにする。

## 目的・スコープ
- 画面全体に表示する背景画像(Texture)の読み込み、保持、描画を担当する。
- 背景アセットのパスと識別子はクラス内部で定義し、識別子を指定して背景を切り替えるための public API を提供する。
- テクスチャのロード失敗時はログ出力するが、リトライや代替テクスチャ配備は行わない（呼び出し元でのフォールバックを想定）。

## ほかのクラスとの関係
- `GameManager` - 初期化時に `BackGroundManager::Initialize()` を呼び出す。
- `WorkPhase`, `SunrisePhase`, `SunsetPhase`, その他フェーズ - 必要に応じて `BackGroundManager::SetBackGround(...)` を呼び出す。

## このクラスが継承するクラス
- なし（static モノステート実装）

## このクラスのコンストラクタ
- デフォルトコンストラクタのみ（利用不可）

## このクラスのデストラクタ
- デフォルトデストラクタのみ（利用不可）

## このクラスに含まれるデータメンバ
- `inline static Array<std::pair<String, Texture>> s_loadedTextures` - ロード済みの背景テクスチャの配列。先頭要素が識別子、2要素目が対応する `Texture`。
- `inline static String s_currentBackgroundId` - 現在描画対象の背景識別子。
- `inline static bool s_initialized` - 初期化済みフラグ。

## このクラスに含まれる定数
- `static constexpr std::array<std::pair<StringView, StringView>, 1> s_backgroundAssets` - (識別子, アセットパス) の定義配列。現在は `("my_room", "Asset/background.png")` を含む。

## このクラスに含まれるpublicメソッド
- `static void Initialize()` - すべての背景アセットを読み込み `s_loadedTextures` を構築する。初回のみ有効。
- `static void SetBackGround(const String& backgroundId)` - 指定された識別子に存在すれば現在の背景を切り替える。存在しなければログ出力する。
- `static void Draw()` - 現在の背景識別子に対応する `Texture` を画面全体へ描画する。背景IDが空なら何もしない。
- `static const String& GetCurrentBackgroundId()` - 現在の背景ID を返す。
- `static void Reset()` - ロード済みテクスチャを解放し、状態を初期状態に戻す（初期背景は `"my_room"`）。

## このクラスに含まれるprivateメソッド
- （なし。全て public static / static データメンバで実装されている）

## このクラスで参照するアセットの情報
- 背景画像: `Asset/background.png`（識別子: `my_room`）

## このクラスが参照する仕様書の項目
- 各フェーズ（`SunrisePhase`, `WorkPhase`, `SunsetPhase` 等）での背景表示

## イメージ図
- 無し（簡単な背景描画のため図は不要）

## このクラスが使用されるフェーズ
- `SunrisePhase`
- `WorkPhase`
- `SunsetPhase`
- その他、背景切り替えが必要なフェーズ

## 特記事項・メモ
- モノステート（static）実装のためスレッド安全性は想定していない。LLM 等のマルチスレッド処理とは分離して利用すること。
- リソース管理はシンプルに `Array::clear()` を行っている。必要なら将来 `Texture` のフォールバックや遅延ロードに拡張可能。
# BackGroundManager 設計書

## 概要
背景画像の読み込み・保持と描画を行うモノステート（static）クラス。ゲーム全体で共有される背景テクスチャを管理し、フェーズから識別子で背景を切り替えられるようにする。

## 目的・スコープ
- 画面全体に表示する背景画像(Texture)の読み込み、保持、描画を担当する。
- 背景アセットのパスと識別子はクラス内部で定義し、識別子を指定して背景を切り替えるための public API を提供する。
- テクスチャのロード失敗時はログ出力するが、リトライや代替テクスチャ配備は行わない（呼び出し元でのフォールバックを想定）。

## ほかのクラスとの関係
- `GameManager` - 初期化時に `BackGroundManager::Initialize()` を呼び出す。
- `WorkPhase`, `SunrisePhase`, `SunsetPhase`, その他フェーズ - 必要に応じて `BackGroundManager::SetBackGround(...)` を呼び出す。

## このクラスが継承するクラス
- なし（static モノステート実装）

## このクラスのコンストラクタ
- デフォルトコンストラクタのみ（利用不可）

## このクラスのデストラクタ
- デフォルトデストラクタのみ（利用不可）

## このクラスに含まれるデータメンバ
- `inline static Array<std::pair<String, Texture>> s_loadedTextures` - ロード済みの背景テクスチャの配列。先頭要素が識別子、2要素目が対応する `Texture`。
- `inline static String s_currentBackgroundId` - 現在描画対象の背景識別子。
- `inline static bool s_initialized` - 初期化済みフラグ。

## このクラスに含まれる定数
- `static constexpr std::array<std::pair<StringView, StringView>, 1> s_backgroundAssets` - (識別子, アセットパス) の定義配列。現在は `("my_room", "Asset/background.png")` を含む。

## このクラスに含まれるpublicメソッド
- `static void Initialize()` - すべての背景アセットを読み込み `s_loadedTextures` を構築する。初回のみ有効。
- `static void SetBackGround(const String& backgroundId)` - 指定された識別子に存在すれば現在の背景を切り替える。存在しなければログ出力する。
- `static void Draw()` - 現在の背景識別子に対応する `Texture` を画面全体へ描画する。背景IDが空なら何もしない。
- `static const String& GetCurrentBackgroundId()` - 現在の背景ID を返す。
- `static void Reset()` - ロード済みテクスチャを解放し、状態を初期状態に戻す（初期背景は `"my_room"`）。

## このクラスに含まれるprivateメソッド
- （なし。全て public static / static データメンバで実装されている）

## このクラスで参照するアセットの情報
- 背景画像: `Asset/background.png`（識別子: `my_room`）

## このクラスが参照する仕様書の項目
- 各フェーズ（`SunrisePhase`, `WorkPhase`, `SunsetPhase` 等）での背景表示

## イメージ図
- 無し（簡単な背景描画のため図は不要）

## このクラスが使用されるフェーズ
- `SunrisePhase`
- `WorkPhase`
- `SunsetPhase`
- その他、背景切り替えが必要なフェーズ

## 特記事項・メモ
- モノステート（static）実装のためスレッド安全性は想定していない。LLM 等のマルチスレッド処理とは分離して利用すること。
- リソース管理はシンプルに `Array::clear()` を行っている。必要なら将来 `Texture` のフォールバックや遅延ロードに拡張可能。
