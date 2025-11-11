# NightDreamPhase設計書

## 概要

夜の夢フェーズを管理するクラス。2日目から3日目へ移行する際にのみ発生し、妹との思い出や妹の存在の大切さを表現するメッセージを表示する。6つの状態(FadeInWait/MessageDisplayInterval/ShowingMessage/FadeOutInterval/FadingOut/PhaseChangeInterval)を持ち、細かいタイミング制御を行う。

## 目的・スコープ

- 2日目から3日目への移行時のみ発生する特別なフェーズを管理する
- 妹との思い出や妹の大切さを表現するメッセージを表示し、プレイヤーの妹への愛着を深める
- 背景、BGM、メッセージ表示、暗転演出を順番に実行する
- メッセージ表示完了後に画面を暗転させ、一日の始まりフェーズへ遷移する

## ほかのクラスとの関係

- `iPhase` - このクラスの基底クラス。update()とdraw()をオーバーライドする
- `MessageWindowUI` - 妹との思い出を示すメッセージ(kDreamMessages)の表示と進行を制御する
- `BlackOutUI` - フェーズ開始時の暗転解除(FadeOut)と終了時の暗転効果(FadeIn)、状態確認(IsHidden/IsVisible)に使用する
- `BackGroundManager` - 背景画像("amusement")の設定に使用する
- `SoundManager` - BGM("bgm_amusement_park")の再生に使用する
- `MouseEffectManager` - マウスエフェクト音の無効化に使用する
- `PhaseManager` - PhaseType::Sunriseへのフェーズ遷移を実行する
- `SunsetPhase` - 2日目の終わりにこのフェーズへ遷移する（SunsetPhaseが判定）
- `SunrisePhase` - このフェーズからメッセージ表示完了後に遷移する

## このクラスが継承するクラス

`iPhase`を継承する

## このクラスのコンストラクタ

`explicit NightDreamPhase()` - コンストラクタ。currentState_をFadeInWaitに設定、背景を"amusement"、BGMを"bgm_amusement_park"に設定、マウスエフェクト音無効化、BlackOutUI::FadeOut(kFadeDuration)で暗転解除開始、intervalTimer_をリセット

## このクラスのデストラクタ

`~NightDreamPhase() override` - デフォルトデストラクタ

## このクラスに含まれるデータメンバ

- `State currentState_` - 現在の状態（初期値FadeInWait）
- `Stopwatch intervalTimer_` - インターバル計測用タイマー（複数の状態で使い回し）

## このクラスに含まれる定数

### 状態定義

- `enum class State` - フェーズ内の状態を表す列挙型
  - `FadeInWait` - 暗転が明けるのを待つ
  - `MessageDisplayInterval` - 暗転明けからメッセージ表示までのインターバル
  - `ShowingMessage` - メッセージ表示中
  - `FadeOutInterval` - メッセージ終了後から暗転開始までのインターバル
  - `FadingOut` - 暗転中
  - `PhaseChangeInterval` - 暗転完了後、フェーズ遷移まで待つ状態

### タイミング関連

- `static constexpr double kFadeDuration = 3.0` - 暗転/明転の所要時間（秒）
- `static constexpr double kMessageDisplayInterval = 2.0` - 暗転明けからメッセージ表示までの待機時間（秒）
- `static constexpr double kFadeOutInterval = 2.0` - メッセージ終了後から暗転開始までの待機時間（秒）
- `static constexpr double kPhaseChangeInterval = 1.5` - 暗転完了後に待つ秒数（秒）

### メッセージ関連

- `static inline const Array<String> kDreamMessages` - 表示するメッセージの配列（11個のメッセージ: 遊園地での思い出、妹の様子、メリーゴーランドの情景など）

## このクラスに含まれるpublicメソッド

- `void update() override` - 毎フレーム呼ばれる更新処理。状態に応じた処理を実行し、フェーズ遷移を管理する
- `void draw() const override` - 描画処理。MessageWindowUIとBlackOutUIが描画するため、ここでは何もしない

## このクラスに含まれるprivateメソッド

- `void StartFadeOut()` - メッセージ表示完了後に暗転を開始する
- `void TransitionToNextPhase()` - SunrisePhaseへ遷移する

## このクラスで参照するアセットの情報

- 背景画像: 部屋の背景（BackGroundManagerで管理、IDは"my_room"を想定）
- BGM: 静かで感情的な音楽（SoundManagerで管理、キーは実装時に決定）

## このクラスが参照する仕様書の項目

- 夜の夢フェーズ
- フェーズ遷移図
- メッセージウィンドウ

## このクラスが使用されるフェーズ

- 夜の夢フェーズ

## 特記事項・メモ

- State列挙型の定義：
  - `FadeInWait`: 暗転が明けるのを待つ状態
  - `ShowingMessage`: メッセージ表示中
  - `FadingOut`: 暗転中
  - `PhaseChangeInterval`: 暗転完了後、フェーズ遷移まで待つ状態
- 処理フロー：
  1. コンストラクタで背景設定、BGM再生、暗転解除（FadeOut）を開始
  2. 暗転が完全に明けるまで待機（FadeInWait）
  3. 暗転が明けたらMessageWindowUIにメッセージを設定し表示開始（ShowingMessage）
  4. プレイヤーがメッセージを全て読み終えるのを待つ
  5. メッセージ完了後、BlackOutUI.FadeIn()で暗転開始（FadingOut）
  6. 暗転完了後、kPhaseChangeInterval秒待機（PhaseChangeInterval）
  7. 待機完了後、BlackOutUI.FadeOut()で明るくし、SunrisePhaseへ遷移
- メッセージ内容の例（実装時に調整）：
  - 「...昔、妹と一緒に見た星空を思い出す。」
  - 「...あの時の笑顔が、今も心に残っている。」
  - 「...妹だけは、俺のことを必要としてくれている。」
- SunsetPhaseのような構造を参考にしつつ、メッセージ内容が異なる
- このフェーズは2日目から3日目への移行時のみ発生する（判定はSunsetPhase側で行う）
