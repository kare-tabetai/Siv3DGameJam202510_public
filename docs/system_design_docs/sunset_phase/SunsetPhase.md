# SunsetPhase設計書

## 概要

一日の終わりフェーズを管理するクラス。「...もう寝よう」というメッセージを表示し、暗転して次の日へ遷移する。4状態管理(FadeInWait→ShowingMessage→FadingOut→PhaseChangeInterval)により、フェードイン待機→メッセージ表示→フェードアウト→一定時間待機の流れを制御する。kNightDreamDay(1日目)の終わりにはNightDreamPhaseへ、それ以外はSunrisePhaseへ遷移し、遷移前にGameCommonData::AdvanceDay()で日数を進める。

## 目的・スコープ

一日の終了を演出し、翌日への遷移を管理する。メッセージ表示後に画面を暗転させ、GameCommonDataの日数を進める。淡々とした演出により、主人公の日常の単調さを表現する。1日目の終わりのみNightDreamPhaseへの特別遷移を実施。

## ほかのクラスとの関係

- `PhaseManager` - ChangePhase(PhaseType::NightDream)またはChangePhase(PhaseType::Sunrise)で次フェーズへ遷移
- `MessageWindowUI` - Show(kSleepMessages)で「...もう寝よう」メッセージを表示。IsVisible()で表示完了を判定
- `BlackOutUI` - FadeOut(kFadeDuration)でフェードアウト、FadeIn(kFadeDuration)でフェードイン。IsHidden()でフェードアウト完了、IsVisible()でフェードイン完了を判定
- `GameCommonData` - GetCurrentDay()で現在日数を取得、AdvanceDay()で日数を進める
- `BackGroundManager` - SetBackGround(U"my_room")で背景設定
- `SoundManager` - PlayBGM(U"bgm_room")でBGM再生
- `MouseEffectManager` - SetActiveSound(false)でマウスエフェクト音無効化
- `SunrisePhase` - 1日目以外の翌朝へ遷移
- `NightDreamPhase` - 1日目の夜(currentDay == kNightDreamDay)の翌朝へ遷移

## このクラスが継承するクラス

`iPhase`を継承する

## このクラスのコンストラクタ

```cpp
explicit SunsetPhase()
```

State::FadeInWaitで初期化。BackGroundManager::SetBackGround(U"my_room")、SoundManager::PlayBGM(U"bgm_room")、MouseEffectManager::SetActiveSound(false)を実行。BlackOutUI::FadeOut(kFadeDuration)でフェードアウト開始、phaseChangeTimer_.reset()でタイマー初期化。

## このクラスのデストラクタ

デフォルトデストラクタ

## このクラスに含まれるデータメンバ

- `State currentState_` - 現在の状態（初期値State::FadeInWait）。FadeInWait/ShowingMessage/FadingOut/PhaseChangeIntervalを遷移
- `Stopwatch phaseChangeTimer_` - フェーズ遷移待機用タイマー。PhaseChangeInterval状態でkPhaseChangeInterval秒経過を判定

## このクラスに含まれる定数

- `static constexpr double kFadeDuration = 1.5` - 暗転の所要時間（秒）
- `static constexpr double kPhaseChangeInterval = 0.5` - 暗転完了後に待つ秒数（秒）
- `static constexpr int32 kNightDreamDay = 1` - NightDreamPhaseへ遷移する日数（1日目の終わり）
- `static inline const Array<String> kSleepMessages = {U"...もう寝よう"}` - 表示するメッセージ配列（要素数1）

## このクラスに含まれるpublicメソッド

- `void update() override` - 毎フレーム呼ばれる更新処理。currentState_に応じて処理分岐：FadeInWait時はBlackOutUI::IsHidden()でフェードアウト完了を確認しメッセージ表示開始、ShowingMessage時はMessageWindowUI::IsVisible()でメッセージ完了を確認しStartFadeOut()呼び出し、FadingOut時はBlackOutUI::IsVisible()でフェードイン完了を確認しPhaseChangeInterval状態へ遷移、PhaseChangeInterval時はphaseChangeTimer_.sF() >= kPhaseChangeIntervalで待機完了を確認しTransitionToNextDay()呼び出し
- `void draw() const override` - 描画処理。MessageWindowUIとBlackOutUIが描画するため、ここでは何もしない

## このクラスに含まれるprivateメソッド

- `void StartFadeOut()` - メッセージ表示完了後に暗転を開始する。currentState_をState::FadingOutに設定し、BlackOutUI::FadeIn(kFadeDuration)呼び出し
- `void TransitionToNextDay()` - GameCommonDataの日数を進め、次のフェーズへ遷移する。GameCommonData::GetCurrentDay()で現在日数取得、GameCommonData::AdvanceDay()で日数進行、BlackOutUI::FadeOut(kFadeDuration)でフェードアウト開始。currentDay == kNightDreamDayならPhaseManager::ChangePhase(PhaseType::NightDream)、それ以外はPhaseManager::ChangePhase(PhaseType::Sunrise)を呼び出し

## このクラスで参照するアセットの情報

なし

## このクラスが参照する仕様書の項目

- 一日の終わりフェーズ
- フェーズ遷移図

## このクラスが使用されるフェーズ

一日の終わりフェーズ

## 特記事項・メモ

- State列挙型の定義：
  - `ShowingMessage`: メッセージ表示中
  - `FadingOut`: 暗転中
- 処理フロー：
  1. コンストラクタでMessageWindowUIにメッセージ設定
  2. メッセージ表示完了を待つ
  3. メッセージ完了後、BlackOutUI.FadeIn()で暗転開始
  4. 暗転完了後、GameCommonData.AdvanceDay()で日数を進める
  5. BlackOutUI.FadeOut()で明るくし、次のフェーズへ遷移
     - 2日目の終わり（currentDay == 2）の場合はNightDreamPhaseへ
     - それ以外の場合はSunrisePhaseへ
- 演出は淡々としており、BGMやSEは使用しない（環境音のみ継続）
- CommonUIは通常通り表示される
