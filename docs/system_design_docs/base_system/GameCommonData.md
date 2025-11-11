# GameCommonData設計書

## 概要

ゲーム進行中ずっと保持するデータを管理するクラス。精神力、残り日数、現在の日付などゲーム全体で共有される状態を保持する。モノステートパターンで実装され、どこからでもアクセス可能。

## 目的・スコープ

ゲームの進行状態を一元管理し、各フェーズやUIクラスから参照・更新できるようにする。ゲーム全体で共有されるパラメーター（精神力、日数など）の整合性を保つことが目的。モノステートパターンにより、インスタンス化せずに静的メソッドで利用可能。個別のフェーズ固有のデータは対象外。

## ほかのクラスとの関係

- `GameManager` - ゲーム開始時にこのクラスのInitialize()を呼び出す
- `CommonUI` - このクラスの静的メソッドを呼び出してデータを取得し、画面に表示する
- 各フェーズクラス（App::ISceneを継承） - このクラスの静的メソッドを呼び出してデータを参照・更新してゲームの進行を制御する

## このクラスが継承するクラス

継承なし（モノステートパターンで実装）

## このクラスのコンストラクタ

モノステートのため不要。実際の初期化は`Initialize()`メソッドで行う。

## このクラスのデストラクタ

モノステートのため不要。

## このクラスに含まれるデータメンバ

- `static inline int32 mentalPower_` - 精神力（0でゲームオーバー、最大100、初期値11）
- `static inline int32 currentDay_` - 現在の経過日数（初期値0）

## このクラスに含まれる定数

- `static constexpr int32 kInitialMentalPower = 11` - 初期精神力
- `static constexpr int32 kMaxMentalPower = 100` - 最大精神力
- `static constexpr int32 kMinMentalPower = 0` - 最小精神力
- `static constexpr int32 kTotalDays = 3` - ゲームの総日数
- `static inline const Date kStartDate{2031, 6, 20}` - ゲーム開始日

## このクラスに含まれるpublicメソッド

- `static void Initialize()` - 静的メンバ変数を初期値にリセットする
- `static void ChangeMentalPower([[maybe_unused]] int32 delta)` - 精神力を増減させる。0～100の範囲に自動的に制限される
- `static void AdvanceDay()` - 日付を1日進める。currentDayを加算する
- `static [[nodiscard]] int32 GetMentalPower() noexcept` - 現在の精神力を取得する
- `static [[nodiscard]] int32 GetRemainingDays() noexcept` - 現在の残り日数を取得する
- `static [[nodiscard]] int32 GetCurrentDay() noexcept` - 現在の経過日数を取得する
- `static [[nodiscard]] bool IsMentalPowerZero() noexcept` - 精神力が0以下かどうかを判定する
- `static [[nodiscard]] bool IsFinalDay() noexcept` - 最終日（残り日数0）かどうかを判定する
- `static [[nodiscard]] bool IsBeforeFinalDay() noexcept` - 最終日の1日前（残り日数1）かどうかを判定する
- `static [[nodiscard]] bool IsFirstDay() noexcept` - 初日（currentDay == 0）かどうかを判定する
- `static [[nodiscard]] String GetCurrentDateString()` - 現在の日付を文字列形式（yyyy/MM/dd）で取得する。開始日2031/06/20からcurrentDayを加算して生成

## このクラスに含まれるprivateメソッド

なし

## このクラスで参照するアセットの情報

なし

## このクラスが参照する仕様書の項目

- 精神力
- フェーズ遷移図
- 一日の始まりフェーズ
- 一日の終わりフェーズ

## このクラスが使用されるフェーズ

全フェーズで使用される（ゲーム全体で共有されるデータ）

## 特記事項・メモ

- モノステートパターンで実装し、静的メンバのみを使用する
- 精神力の変更は必ずChangeMentalPowerメソッドを通して行い、範囲チェックを保証する
- 日付の計算は簡易的に日数を加算する方式を採用（2031/06/20から開始、3日間で2031/06/22まで）
- 日付文字列はGetCurrentDateString()で動的に生成されるため、データメンバとしては保持しない
- currentDayは0から開始し、AdvanceDay()で1ずつ増加する
- remainingDaysはGetRemainingDays()で総日数から計算されるため、データメンバとしては保持しない
- IsFirstDay()はWorkPhaseなどで初回判定に使用される
- ゲーム開始時に`Initialize()`を必ず呼び出す必要がある（GameManagerのコンストラクタで実行）
- kInitialMentalPowerは11に設定され、バランス調整済み
