# SisterMessageUIManager設計書

## 概要

`SisterMessageUI`のインスタンスを所有・管理するstaticクラス。ゲーム全体で単一のSisterMessageUIインスタンスを共有し、各フェーズに`std::weak_ptr`を提供する。

## 目的・スコープ

- `SisterMessageUI`のインスタンスを生成・所有する
- 各フェーズに`SisterMessageUI`への`std::weak_ptr`を提供する
- `SisterMessageUI`の更新・描画処理を一元管理する
- SisterMessageUIのライフサイクルを管理する

## ほかのクラスとの関係

- `SisterMessageUI` - このクラスが所有するインスタンス
- `SisterMessagePhase` - `GetSisterMessageUI()`で`std::weak_ptr`を取得して使用する
- `BadEndPhase` - `GetSisterMessageUI()`で`std::weak_ptr`を取得して使用する
- `GameManager` - 初期化時に`Initialize()`を呼び出し、メインループで`Update()`と`Draw()`を呼び出す

## このクラスが継承するクラス

なし(staticクラス)

## このクラスのコンストラクタ

なし(staticクラスのため、コンストラクタは削除される想定)

## このクラスのデストラクタ

なし(staticクラスのため、デストラクタは削除される想定)

## このクラスに含まれるデータメンバ

- `static inline std::shared_ptr<SisterMessageUI> sisterMessageUI_` - SisterMessageUIのインスタンスを所有するshared_ptr

## このクラスに含まれる定数

なし

## このクラスに含まれるpublicメソッド

- `static void Initialize()` - sisterMessageUI_ = std::make_shared<SisterMessageUI>()でSisterMessageUIのインスタンスを生成。ゲーム開始時に一度だけ呼ばれる（GameManager::Initialize()から）
- `static void Update()` - sisterMessageUI_がnullでなければsisterMessageUI_->Update()を呼び出す。毎フレーム呼ばれる（GameManager::Update()から）
- `static void Draw()` - sisterMessageUI_がnullでなければsisterMessageUI_->Draw()を呼び出す。毎フレーム呼ばれる（GameManager::Draw()から）
- `[[nodiscard]] static std::weak_ptr<SisterMessageUI> GetSisterMessageUI() noexcept` - sisterMessageUI_をstd::weak_ptrとして返す。各フェーズがこのメソッドでインスタンスへの参照を取得し、lock()でアクセス

## このクラスに含まれるprivateメソッド

なし

## このクラスで参照するアセットの情報

なし(SisterMessageUI側で管理)

## このクラスが参照する仕様書の項目

- 妹メッセージフェーズ
- バッドエンドフェーズ
- フェーズごとのUI

## イメージ図

なし

## このクラスが使用されるフェーズ

- ゲーム全体(初期化から終了まで常駐)

## 特記事項・メモ

- このクラスはstaticクラス(モノステート)として実装される
- `SisterMessageUI`のインスタンスは`Initialize()`で生成され、プログラム終了まで保持される
- 各フェーズは`GetSisterMessageUI()`で取得した`std::weak_ptr`を保持し、必要時に`lock()`してアクセスする
- `Update()`と`Draw()`は`GameManager`のメインループから呼ばれる想定
- nullチェックを行い、インスタンスが存在する場合のみ処理を実行する
- このクラスの責務はSisterMessageUIの所有と提供のみで、LLMモデルの初期化自体はSisterMessageUIが行う
