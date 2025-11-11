# GameClearPhase設計書

## 概要

ゲームクリア時のエンディングフェーズを管理するクラス。採用が決まった後の主人公の辟易したセリフを表示し、暗転して終了する。5状態管理(WaitingBeforeMessage→ShowingMessage→FadingOut→WaitingAfterFade→Finished)により、メッセージ表示前待機→メッセージ表示→暗転→暗転後待機→END表示の流れを制御する。

## 目的・スコープ

就職が決まったにもかかわらず、主人公が幸福を感じていないことを表現する。「やっと終わったか」「これで、また働ける...」という辟易したセリフにより、良くも悪くもこのまま日常が続いていくということを表現する。

## ほかのクラスとの関係

- `MessageWindowUI` - Update()で更新、Show(kEndingMessages)でメッセージ表示、IsCompleted()で表示完了判定、Hide()で非表示化
- `BlackOutUI` - FadeIn(kFadeDuration)で暗転開始、IsVisible()で暗転完了判定、SetMessage(kEndMessage)でEND表示
- `SoundManager` - PlayBGM(U"bgm_room")で室内環境音再生、PlayBGM(U"bgm_clear")でクリアBGM再生
- `MouseEffectManager` - SetActiveSound(false)でマウスエフェクト音無効化
- `JobSearchPhase` - 採用判定後、このフェーズへ遷移

## このクラスが継承するクラス

`iPhase`を継承する

## このクラスのコンストラクタ

```cpp
explicit GameClearPhase()
```

SoundManager::PlayBGM(U"bgm_room")で室内環境音再生、MouseEffectManager::SetActiveSound(false)でマウスエフェクト音無効化。initialDelayStopwatch_.start()で待機タイマー開始、currentState_をState::WaitingBeforeMessageに設定。

## このクラスのデストラクタ

デフォルトデストラクタ

## このクラスに含まれるデータメンバ

- `State currentState_` - 現在の状態（初期値State::WaitingBeforeMessage）。WaitingBeforeMessage/ShowingMessage/FadingOut/WaitingAfterFade/Finishedを遷移
- `Stopwatch initialDelayStopwatch_` - メッセージ表示前の待機時間計測用。WaitingBeforeMessage状態でkInitialDelay秒経過を判定
- `double fadeCompleteTime_` - 暗転完了時刻の記録(Scene::Time())。WaitingAfterFade状態でkEndMessageDelay秒経過を判定

## このクラスに含まれる定数

- `static constexpr double kInitialDelay = 0.5` - メッセージ表示前の待機時間(秒)
- `static constexpr double kFadeDuration = 2.0` - 暗転の所要時間（秒）
- `static constexpr double kEndMessageDelay = 1.5` - 暗転完了後にGAME CLEARを表示するまでの待ち時間(秒)
- `static inline const Array<String> kEndingMessages` - 表示するメッセージ配列。要素数4: "...", "やっと終わったか...", "これで、また働ける...", "..."
- `static inline String kEndMessage = U"END1:社会の歯車"` - 暗転上に表示するメッセージ

## このクラスに含まれるpublicメソッド

- `void update() override` - 毎フレーム呼ばれる更新処理。MessageWindowUI::Update()を呼び出し。currentState_に応じて処理分岐：WaitingBeforeMessage時はinitialDelayStopwatch_.sF() >= kInitialDelayでMessageWindowUI::Show(kEndingMessages)呼び出し、ShowingMessage時はMessageWindowUI::IsCompleted()でメッセージ完了を確認しStartFadeOut()呼び出し、FadingOut時はBlackOutUI::IsVisible()で暗転完了を確認しfadeCompleteTime_記録とWaitingAfterFade状態へ遷移、WaitingAfterFade時は(Scene::Time() - fadeCompleteTime_) >= kEndMessageDelayでBlackOutUI::SetMessage(kEndMessage)呼び出しFinished状態へ遷移、Finished時はBlackOutUI::SetMessage(kEndMessage)を保持
- `void draw() const override` - 描画処理。MessageWindowUIとBlackOutUIが自動的に描画されるため、ここでは何もしない

## このクラスに含まれるprivateメソッド

- `void StartFadeOut()` - MessageWindowUI::Hide()でメッセージ非表示化、SoundManager::PlayBGM(U"bgm_clear")でクリアBGM再生、BlackOutUI::FadeIn(kFadeDuration)で暗転開始、currentState_をState::FadingOutに設定

## このクラスで参照するアセットの情報

- BGM: `Asset/bgm_room.mp3` - 室内の環境音（継続）

## このクラスが参照する仕様書の項目

- エンディングフェーズ
- フェーズ遷移図
- 演出

## このクラスが使用されるフェーズ

エンディングフェーズ

## 特記事項・メモ

- State列挙型の定義：
  - `ShowingMessage`: メッセージ表示中
  - `FadingOut`: 暗転中
  - `Finished`: 終了（暗転完了後、ゲーム終了またはタイトルへ）
- エンディングメッセージ例：
  - 「やっと終わったか...」
  - 「これで、また働ける...」
  - 「...」
- 処理フロー：
  1. コンストラクタでMessageWindowUIにメッセージ設定、環境音を継続再生
  2. メッセージ表示完了を待つ
  3. メッセージ完了後、BlackOutUI.FadeIn()で暗転開始
  4. 暗転完了後、State::Finishedへ遷移
  5. Finished状態では何もせず、プレイヤーがウィンドウを閉じるのを待つ
- 曲は変えずに環境音のまま（仕様書の要件）
- CommonUIは非表示
- 暗転後はそのままプレイヤーがゲームを閉じるのを待つ
- 環境音の再生はSoundManagerクラスを通じて行う
