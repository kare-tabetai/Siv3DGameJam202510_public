# IntroductionPhase設計書

## 概要

ゲーム開始時の初回のみ実行される導入フェーズを管理するクラス。世界観の説明を黒画面に白文字で表示し、クリックで次のテキストへ進む。BlackOutUIにメッセージ表示を依頼する形で実装。

## 目的・スコープ

プレイヤーにゲームの世界観を必要最低限伝える役割を担う。タイトル画面を省略し、起動後すぐにゲームを開始できるよう、簡潔な説明のみを行う。クリック待機でテキストを順次表示し、全テキスト表示後にSunrisePhaseへ自動遷移する。導入フェーズ以外の処理は対象外。

## ほかのクラスとの関係

- `iPhase` - このクラスの基底クラス
- `BlackOutUI` - BlackOutUI::ShowImmediately()で即座に暗転、SetMessage()でテキスト設定、ClearMessage()でメッセージクリア、FadeOut()でフェードアウトを依頼
- `PhaseManager` - PhaseType::Sunriseへのフェーズ遷移を実行
- `MouseEffectManager` - マウスエフェクト音の無効化に使用
- `SunrisePhase` - 導入完了後、このフェーズへ遷移する

## このクラスが継承するクラス

`iPhase`を継承する

## このクラスのコンストラクタ

`explicit IntroductionPhase()` - コンストラクタ。displayTimer_.start()でタイマー開始、BlackOutUI::ShowImmediately()で即座に暗転、ShowNextText()で最初のテキスト表示、MouseEffectManager::SetActiveSound(false)でマウスエフェクト音無効化

## このクラスのデストラクタ

`~IntroductionPhase() override` - デフォルトデストラクタ

## このクラスに含まれるデータメンバ

- `Array<String> introductionTexts_` - 導入で表示するテキスト配列（4つのテキスト: 注意書き、空行、2031年の説明、モノアイの説明）
- `int32 currentTextIndex_` - 現在表示中のテキストインデックス（初期値0）
- `Stopwatch displayTimer_` - テキスト表示時間計測用タイマー（現状未使用だが将来の自動進行用）
- `bool isWaitingForInput_` - プレイヤーの入力待ち状態かどうか（初期値false）

## このクラスに含まれる定数

- `static constexpr double kFadeDuration = 1.0` - フェードイン・アウトの時間（秒）

## このクラスに含まれるpublicメソッド

- `void update() override` - 毎フレーム呼ばれる更新処理。テキストの切り替えとフェーズ遷移を管理する
- `void draw() const override` - 描画処理。BlackOutUIが描画を担当するため、ここでは何もしない

## このクラスに含まれるprivateメソッド

- `void ShowNextText()` - 次のテキストをBlackOutUIに設定して表示する
- `void TransitionToNextPhase()` - 次のフェーズ（SunrisePhase）へ遷移する。BlackOutUIをフェードアウトしてから遷移

## このクラスで参照するアセットの情報

なし

## このクラスが参照する仕様書の項目

- 導入フェーズ（初回のみ）
- フェーズ遷移図

## イメージ図

![導入フェーズのイメージ図](../spec_image/introduction_phase.png)

## このクラスが使用されるフェーズ

導入フェーズ

## 特記事項・メモ

- 導入テキスト例：
  - 「2030年、日本では統合人材AI管理法とその関連法が施行され、すべての企業は日本政府中央採用AIシステム　モノアイを通してのみ採用を行えるようになった。」
- テキストはクリックを検知して切り替わる（タイマーによる自動進行はなし）
- すべてのテキスト表示完了後、BlackOutUIをフェードアウトしてから自動的にSunrisePhaseへ遷移する
- CommonUIは非表示（BlackOutUIのみ使用）
- 黒画面に白文字という演出はBlackOutUIで実現
