# BadEndPhase設計書

## 概要

最終日に不採用となった際のバッドエンドフェーズを管理するクラス。AI妹ボットサービス終了通知とカーテンのメッセージで、家族（妹）という人間的存在を失う辛さを表現する。7状態管理(NotificationInterval→ShowingNotification→InputInterval→ShowingCurtain→FadingOut→WaitingToShowEnd→Finished)により、通知表示→入力待機→カーテンメッセージ→暗転→END表示の流れを制御する。

## 目的・スコープ

妹の正体がAIボットであったことを明かし、そのサービス終了によって主人公が唯一の心の支えを失うという辛さを表現する。G線上のアリア(bgm_badend)により儚さを演出し、テーマである「AIによって合理化された社会の冷たさ」と「家族という人間的存在の温かさとそれを失う辛さ」を強調する。

## ほかのクラスとの関係

- `SisterMessageUI` - SisterMessageUIManager::GetSisterMessageUI().lock()で取得。SetActive(true)で表示、SetInputAreaDisabled(true)で入力無効化、AddSisterMessage(kServiceEndNotification)で通知表示
- `SisterMessageUIManager` - GetSisterMessageUI()でstd::weak_ptr<SisterMessageUI>を取得
- `MessageWindowUI` - Show(kCurtainMessage)でカーテンメッセージ表示、IsVisible()で表示完了判定
- `BlackOutUI` - FadeIn(kFadeDuration)で暗転開始、IsVisible()で暗転完了判定、SetMessage(kEndMessage)でEND表示
- `SoundManager` - PlayBGM(U"bgm_room")で室内環境音、PlayBGM(U"bgm_badend")でG線上のアリア再生
- `BackGroundManager` - SetBackGround(U"my_room")で背景設定
- `MouseEffectManager` - SetActiveSound(false)でマウスエフェクト音無効化
- `JobSearchPhase` - 最終日に不採用判定後、このフェーズへ遷移

## このクラスが継承するクラス

`iPhase`を継承する

## このクラスのコンストラクタ

```cpp
explicit BadEndPhase()
```

BackGroundManager::SetBackGround(U"my_room")、SoundManager::PlayBGM(U"bgm_room")、MouseEffectManager::SetActiveSound(false)を実行。SisterMessageUIManager::GetSisterMessageUI().lock()でSisterMessageUI取得し、SetActive(true)で表示、SetInputAreaDisabled(true)で入力エリア無効化。inputTimer_.restart()でタイマー開始、currentState_をState::NotificationIntervalに設定。

## このクラスのデストラクタ

デフォルトデストラクタ

## このクラスに含まれるデータメンバ

- `State currentState_` - 現在の状態（初期値State::NotificationInterval）。NotificationInterval/ShowingNotification/InputInterval/ShowingCurtain/FadingOut/WaitingToShowEnd/Finishedを遷移
- `Stopwatch inputTimer_` - コンストラクタ後の入力間隔タイマー。NotificationInterval状態とInputInterval状態で使用
- `Stopwatch endTimer_` - END表示までの待機タイマー。WaitingToShowEnd状態でkEndDisplayDelay秒経過を判定

## このクラスに含まれる定数

- `static constexpr double kFadeDuration = 6.0` - 暗転の所要時間（秒）
- `static constexpr double kNotificationIntervalDuration = 4.0` - 通知表示前の待機時間（秒）
- `static constexpr double kNotificationDisplayDuration = 8.0` - 通知表示時間（秒）
- `static constexpr double kInputIntervalDuration = 1.0` - 通知前の入力間隔（秒）
- `static constexpr double kEndDisplayDelay = 6.0` - 暗転完了後にENDを表示するまでの待機時間（秒）
- `static inline const String kServiceEndNotification` - サービス終了通知文章（【重要】AI妹ボットサービス終了のお知らせ...2031年6月23日）
- `static inline const Array<String> kCurtainMessage` - カーテンメッセージ配列。要素数3: "...", "......", "........"
- `static inline const String kEndMessage = U"END2:一人ぼっち"` - 暗転後に表示するENDメッセージ

## このクラスに含まれるpublicメソッド

- `void update() override` - 毎フレーム呼ばれる更新処理。currentState_に応じて処理分岐：NotificationInterval時はinputTimer_.sF() >= kNotificationIntervalDurationでShowNotification()呼び出し、InputInterval時はinputTimer_.sF() >= kInputIntervalDurationでShowingNotification状態へ遷移、ShowingNotification時はMouseL.down()でShowCurtainMessage()呼び出し、ShowingCurtain時はMessageWindowUI::IsVisible()でメッセージ完了を確認しStartFade()呼び出し、FadingOut時はBlackOutUI::IsVisible()で暗転完了を確認しWaitingToShowEnd状態へ遷移、WaitingToShowEnd時はendTimer_.sF() >= kEndDisplayDelayでBlackOutUI::SetMessage(kEndMessage)呼び出しFinished状態へ遷移
- `void draw() const override` - 描画処理。GameManagerが所有するSisterMessageUI、MessageWindowUI、BlackOutUIが描画を担当するため、ここではフェーズ固有の描画を行わない

## このクラスに含まれるprivateメソッド

- `void ShowNotification()` - SisterMessageUIManager::GetSisterMessageUI().lock()でSisterMessageUI取得し、AddSisterMessage(kServiceEndNotification)呼び出しでサービス終了通知を表示。inputTimer_.restart()でタイマーリセット、currentState_をState::InputIntervalに設定
- `void ShowCurtainMessage()` - MessageWindowUI::Show({kCurtainMessage})を呼び出してカーテンのメッセージを表示。currentState_をState::ShowingCurtainに設定
- `void StartFade()` - SoundManager::PlayBGM(U"bgm_badend")でG線上のアリアBGM再生、currentState_をState::FadingOutに設定、BlackOutUI::FadeIn(kFadeDuration)で暗転開始

## このクラスで参照するアセットの情報

- BGM: `Asset/bgm_badend.mp3` - バッドエンド時のG線上のアリア（音質悪め、儚さを演出）
- SE: `Asset/se_message.wav` - 通知音（LINE風UIのメッセージ送信音を使いまわし）

## このクラスが参照する仕様書の項目

- バッドエンドフェーズ
- フェーズ遷移図
- 妹について
- 演出

## このクラスが使用されるフェーズ

バッドエンドフェーズ

## 特記事項・メモ

- State列挙型の定義：
  - `ShowingNotification`: サービス終了通知表示中
  - `ShowingCurtain`: カーテンメッセージ表示中
  - `FadingOut`: 暗転中
  - `Finished`: 終了（暗転完了後、ゲーム終了またはタイトルへ）
- サービス終了通知の全文（仕様書より）：

  ```text
  【重要】AI妹チャットボットサービス終了のお知らせ

  平素より「AI妹」チャットボットサービスをご愛顧いただき、誠にありがとうございます。

  このたび、AI妹チャットボットサービスは、2031年6月23日（本日）をもちまして、全てのサービスを終了させていただくこととなりました。
  長きにわたりご愛顧いただき、誠にありがとうございました。

  AI妹運営チーム一同
  2031年6月23日
  ```

- 処理フロー：
  1. コンストラクタでサービス終了通知を表示し、状態を ShowingNotification に設定
  2. SoundManager::PlaySE("se_message")で通知音を鳴らす
  3. コンストラクタで受け取った`SisterMessageUI`の`std::weak_ptr`を`lock()`し、得られた`shared_ptr`に対して`AddSisterMessage()`を呼び出してサービス終了通知を表示
  4. 通知表示中はクリック待ち
  5. クリックでMessageWindowUI::Show()を呼び出して「カーテンが風でなびいている。」を表示
  6. カーテンメッセージ完了後、SoundManager::PlayBGM("bgm_badend")でG線上のアリアBGM開始
  7. BlackOutUI::FadeIn()を呼び出してゆっくりと暗転開始
  8. 暗転完了後、State::Finishedへ遷移
  9. Finished状態では何もせず、プレイヤーがウィンドウを閉じるのを待つ
- 妹の正体がAIであることはバッドエンド以外では明かされない（仕様書の要件）
- G線上のアリアは音質を意図的に悪くしたファイルを使用し、儚さ・虚しさを演出
- CommonUIは非表示
- このエンディングは最終日（6/23）に不採用となった場合のみ発生
