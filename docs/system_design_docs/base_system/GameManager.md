# GameManager設計書

## 概要

ゲーム全体を管理する静的クラス。各フェーズの管理、共通データの保持、共通UIの統合を担当する。モノステートパターンで実装され、Initialize()で初期化、Update()/Draw()でゲームループを実行する。

## 目的・スコープ

ゲームのメインループを制御し、PhaseManagerを通じて現在のフェーズに応じた更新・描画を行う。フェーズ間の遷移、GameCommonDataの管理、MessageWindowUIやBlackOutUIなどの共通UIの統合、SisterMessageUIManagerの統合、各種マネージャーの初期化を担当する。個別フェーズの詳細な処理は対象外。

## ほかのクラスとの関係

- `PhaseManager` - 各フェーズの管理、RegisterPhaseFactory()での登録、ChangePhase()での遷移、Update()/Draw()/PostDraw()の呼び出しを行う
- `GameCommonData` - モノステートパターンで実装された共通データをInitialize()で初期化
- `CommonUI` - モノステートパターンで実装された共通UIをInitialize()とDraw()で管理
- `MessageWindowUI` - モノステートパターンで実装されたメッセージウィンドウUIをInitialize()、Update()、Draw()で管理
- `BlackOutUI` - モノステートパターンで実装された暗転UIをInitialize()、Update()、Draw()で管理
- `SisterMessageUIManager` - Initialize()、Update()、Draw()を呼び出してSisterMessageUIを管理
- `FontManager` - Initialize()でフォントシステムを初期化
- `SoundManager` - Initialize()とUpdate()を呼び出してサウンドシステムを管理（クロスフェード処理のため必須）
- `BackGroundManager` - Initialize()とDraw()を呼び出して背景を管理
- `MouseEffectManager` - Initialize()、Update()、Draw()を呼び出してマウスエフェクトを管理
- `DebugSystem` - デバッグビルド時のみ、Update()とDraw()を呼び出してデバッグ表示を管理
- 各Phaseクラス（iPhaseを継承） - RegisterPhaseFactories()でPhaseManagerに登録され、実行時に生成・管理される
- `Main.cpp` - GameManager::Initialize()、Update()、Draw()を呼び出してゲームループを実行

## このクラスが継承するクラス

継承なし（静的クラスとして実装）

## このクラスのコンストラクタ

インスタンス化不要。実際の初期化は`Initialize()`静的メソッドで行う。

## このクラスのデストラクタ

`~GameManager()` - デフォルトデストラクタ

## このクラスに含まれるデータメンバ

なし（全て静的メソッドで管理）

## このクラスに含まれる定数

なし

## このクラスに含まれるpublicメソッド

- `static void Initialize()` - ゲーム全体の初期化処理。FontManager、SoundManager、BackGroundManager、GameCommonData、CommonUI、MessageWindowUI、BlackOutUI、SisterMessageUIManager、PhaseManager、MouseEffectManagerを初期化し、RegisterPhaseFactories()でフェーズ登録後、GameConst::kInitialPhaseで初期フェーズを設定
- `static void Update()` - ゲーム全体の更新処理。MouseEffectManager、SoundManager(クロスフェード処理のため必須)、BlackOutUI、MessageWindowUI、SisterMessageUIManager、PhaseManager、DebugSystem(デバッグビルドのみ)を順に更新
- `static void Draw()` - ゲーム全体の描画処理。BackGroundManager、SisterMessageUIManager、PhaseManager、CommonUI、MessageWindowUI、BlackOutUI、PhaseManager::PostDraw()、MouseEffectManager、DebugSystem(デバッグビルドのみ)を順に描画
- `~GameManager()` - デストラクタ

## このクラスに含まれるprivateメソッド

- `static void RegisterPhaseFactories()` - 各PhaseTypeに対応する生成関数をPhaseManager::RegisterPhaseFactory()で登録。Introduction、Sunrise、JobSearch、SisterMessage、Work、Sunset、NightDream、GameOver、BadEnd、GameClearの10フェーズを登録

## このクラスで参照するアセットの情報

なし（各UIクラスやフェーズクラスが個別に参照）

## このクラスが参照する仕様書の項目

- ゲーム概要
- フェーズ遷移図
- 共通UI
- 各フェーズ詳細

## このクラスが使用されるフェーズ

全フェーズ（ゲーム全体を管理）

## 特記事項・メモ

- 静的クラスとして実装され、インスタンス化不要
- Initialize()はMain.cppから1度だけ呼び出される
- Update()とDraw()は毎フレームMain.cppから呼び出される
- フェーズはPhaseTypeで識別され、PhaseManagerが実際の管理を担当
- 登録されるフェーズ: Introduction, Sunrise, JobSearch, SisterMessage, Work, Sunset, NightDream, GameOver, BadEnd, GameClear
- CommonUIは特定のフェーズ（Introduction, GameClear, GameOver, BadEnd）では各フェーズ側で描画をスキップする
- BlackOutUIは最前面に描画され、他のすべての要素の上に重なる
- PhaseManager::PostDraw()は暗転の手前の描画を可能にする（現状未使用だが将来の拡張用）
- 60FPS動作を前提とした設計
- **SoundManager::Update()の呼び出しは必須**: BGMのクロスフェード処理を正常に機能させるため、GameManager::Update()内で毎フレーム必ずSoundManager::Update()を呼び出す
- SisterMessageUIはSisterMessageUIManagerが管理し、各フェーズはSisterMessageUIManager::GetSisterMessageUI()でweak_ptrを取得してlock()で利用
- デバッグビルド時のみDebugSystemが有効化され、F10キーでデバッグ表示の切り替えが可能
- 描画順序: 背景 → SisterMessageUI → Phase → CommonUI → MessageWindowUI → BlackOutUI → PostDraw → MouseEffect → Debug
- 更新順序: MouseEffect → SoundManager → BlackOutUI → MessageWindowUI → SisterMessageUIManager → PhaseManager → Debug
