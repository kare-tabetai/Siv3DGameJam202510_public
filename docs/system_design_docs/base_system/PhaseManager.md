# PhaseManager設計書

## 概要
フェーズの管理と切り替えを行う静的クラス。循環includeを避けるため、各Phaseの生成処理は外部から登録する方式（ファクトリパターン）を採用している。

## 目的・スコープ
ゲームの各フェーズ（導入、朝、求職活動、妹メッセージ、アルバイト、夕方、夜の夢、ゲームオーバー、バッドエンド、ゲームクリア）の生成、管理、切り替えを担当する。現在のフェーズの更新と描画を制御する。

## ほかのクラスとの関係
- `iPhase` - このインターフェースを実装した各フェーズクラスを管理する
- `PhaseType` - フェーズの種別を示す列挙型
- `GameManager` - このクラスを使用してフェーズを管理する
- 各フェーズクラス（IntroductionPhase、SunrisePhase、JobSearchPhase等） - GameManagerから生成関数が登録され、フェーズ切り替え時に生成される

## このクラスが継承するクラス
なし（静的クラス）

## このクラスのコンストラクタ
インスタンス化を禁止（コンストラクタは削除）

## このクラスのデストラクタ
なし

## このクラスに含まれるデータメンバ
- `static inline std::shared_ptr<iPhase> currentPhase_` - 現在のフェーズのインスタンス
- `static inline PhaseType currentPhaseType_ = PhaseType::Introduction` - 現在のフェーズ種別
- `static inline HashTable<PhaseType, PhaseFactory> phaseFactories_` - PhaseType毎の生成関数を保持するハッシュテーブル

## このクラスに含まれる定数
なし

## このクラスに含まれるpublicメソッド
- `using PhaseFactory = std::function<std::shared_ptr<iPhase>()>` - Phase生成関数の型定義
- `static void Initialize()` - 初期化処理。phaseFactories_をクリアし、currentPhase_をリセットする
- `static void RegisterPhaseFactory(PhaseType phaseType, PhaseFactory factory)` - Phase生成関数を登録する
- `static void Update()` - 毎フレームの更新処理。currentPhase_のupdateメソッドを呼び出す
- `static void Draw()` - 毎フレームの描画処理。currentPhase_のdrawメソッドを呼び出す
- `static void PostDraw()` - 暗転の手前に描画するものがあればここで描画。currentPhase_のpostDrawメソッドを呼び出す
- `static void ChangePhase(PhaseType phaseType)` - フェーズを変更する。登録されたファクトリ関数を実行してPhaseを生成する
- `[[nodiscard]] static PhaseType GetCurrentPhaseType()` - 現在のPhaseTypeを取得する
- `[[nodiscard]] static String GetCurrentPhaseName()` - 現在のフェーズ名を取得する
- `[[nodiscard]] static PhaseType GetNextPhaseType()` - 現在の次のPhaseTypeを取得する（列挙の順序に従い最後は先頭に戻る）

## このクラスに含まれるprivateメソッド
なし

## このクラスで参照するアセットの情報
なし

## このクラスが参照する仕様書の項目
- フェーズ管理システム
- 各フェーズの仕様

## イメージ図
無し

## このクラスが使用されるフェーズ
- 全フェーズ（フェーズ管理の中核となるクラス）

## 特記事項・メモ
- 循環includeを避けるため、各Phaseクラスのヘッダーは直接includeせず、生成関数をGameManagerから登録する方式を採用
- ファクトリパターンを使用してフェーズのインスタンスを動的に生成する
- 静的クラスとして実装され、インスタンス化は禁止される
