# GameOverPhase設計書

## 概要

精神力が0になった際のゲームオーバーフェーズを管理するクラス。正気を失ったような赤文字のセリフとホワイトノイズで辛さを表現する。5状態管理(ShowingMessage→FadingOut→WaitingAfterFade→WaitLogo→Finished)により、メッセージ表示→暗転→暗転後待機→ロゴ表示待機→モノアイ表示の流れを制御する。

## 目的・スコープ

主人公が精神的に追い詰められ、正気を失った状態を表現する。赤文字のメッセージとホワイトノイズBGMにより、不穏で狂気的な雰囲気を演出する。Finished状態では赤いモノアイアイコンを画面中央上1/3に表示し、監視社会の恐怖を表現する。プレイヤーに強い印象を与え、精神力管理の重要性を感じさせる。

## ほかのクラスとの関係

- `MessageWindowUI` - Update()で更新、Show(kGameOverMessages)でメッセージ表示、IsCompleted()で表示完了判定、Hide()で非表示化、SetTextColor(kRedTextColor)で赤文字設定
- `BlackOutUI` - Update()で更新、FadeIn(kFadeDuration)で暗転開始、IsVisible()で暗転完了判定、SetTextColor(kRedTextColor)でEND表示色設定、SetMessage(kEndMessage)でEND表示
- `SoundManager` - PlayBGM(U"bgm_gameover", SoundManager::kDefaultBGMVolume)でホワイトノイズBGM再生
- `MouseEffectManager` - SetActiveSound(false)でマウスエフェクト音無効化
- `JobSearchPhase` / `EventPhase` など - 精神力が0になった時点で、このフェーズへ遷移

## このクラスが継承するクラス

`iPhase`を継承する

## このクラスのコンストラクタ

```cpp
explicit GameOverPhase() : redIcon_(kRedIconPath)
```

初期化リストでredIcon_(kRedIconPath)を初期化。SoundManager::PlayBGM(U"bgm_gameover", SoundManager::kDefaultBGMVolume)でホワイトノイズBGM再生、MessageWindowUI::Show(kGameOverMessages)でメッセージ表示、MessageWindowUI::SetTextColor(kRedTextColor)で赤文字設定、MouseEffectManager::SetActiveSound(false)でマウスエフェクト音無効化。currentState_をState::ShowingMessageに設定、messageCompleted_をfalseに初期化。

## このクラスのデストラクタ

デフォルトデストラクタ

## このクラスに含まれるデータメンバ

- `State currentState_` - 現在の状態（初期値State::ShowingMessage）。ShowingMessage/FadingOut/WaitingAfterFade/WaitLogo/Finishedを遷移
- `bool messageCompleted_` - メッセージ表示完了フラグ（初期値false）。ShowingMessage状態で一度だけtrueに設定
- `Stopwatch stopWatch_` - 暗転完了からの経過時間計測用タイマー。WaitingAfterFade状態とWaitLogo状態で使用
- `Texture redIcon_` - Finished時に表示する赤アイコンのテクスチャ（kRedIconPathから読み込み）

## このクラスに含まれる定数

- `static constexpr double kFadeDuration = 1.5` - 暗転の所要時間（秒）
- `static constexpr double kEndMessageDelay = 1.5` - 暗転完了後にENDを表示するまでの待ち時間（秒）
- `static constexpr double kWaitLogoDuration = 3.0` - ロゴ表示待機から終了までの時間（秒）
- `static inline const ColorF kRedTextColor{0.9, 0.1, 0.1}` - 赤文字の色（MessageWindowUIのテキスト色として設定）
- `static inline const Array<String> kGameOverMessages` - 表示するメッセージ配列。要素数6: "ア..ア......アアアアアアッァァァァァァァァァ", "もう...何も...かも...", "ダメだ...全て...", "誰にも......必要とされていない.........", "自分は...いる意味がないんだ...", "......"
- `static inline const String kEndMessage = U"GAME OVER:モノアイはあなたを見ています"` - 暗転後に表示するENDメッセージ
- `static constexpr StringView kRedIconPath = U"Asset/monoEye_icon_red.png"` - モノアイの赤アイコンパス
- `static constexpr Vec2 kRedIconAnchor{0.5, 1.0 / 3.0}` - アイコン描画用のアンカー位置（画面幅に対する相対位置）
- `static constexpr double kRedIconDisplaySize{160.0}` - アイコン表示サイズ（ピクセル）
- `static constexpr Vec2 kRedIconCenter{GameConst::kWindowSize.x / 2, GameConst::kWindowSize.y * 1.0 / 3.0}` - アイコン描画位置（画面中央上1/3）

## このクラスに含まれるpublicメソッド

- `void update() override` - 毎フレーム呼ばれる更新処理。MessageWindowUI::Update()を呼び出し。currentState_に応じて処理分岐：ShowingMessage時はMessageWindowUI::IsCompleted()でメッセージ完了を確認しmessageCompleted_をtrueに設定してStartFadeOut()呼び出し、FadingOut時はBlackOutUI::Update()呼び出し後BlackOutUI::IsVisible()で暗転完了を確認しstopWatch_.restart()とWaitingAfterFade状態へ遷移、WaitingAfterFade時はstopWatch_.sF() >= kEndMessageDelayでBlackOutUI::SetTextColor(kRedTextColor)とBlackOutUI::SetMessage(kEndMessage)呼び出しWaitLogo状態へ遷移、WaitLogo時はstopWatch_.sF() >= kWaitLogoDurationでFinished状態へ遷移
- `void draw() const override` - 描画処理。何もしない（MessageWindowUIとBlackOutUIが自動描画）
- `void postDraw() const override` - 追加描画処理。currentState_ == State::FinishedならredIcon_.resized(kRedIconDisplaySize).draw(Arg::center = kRedIconCenter)で赤いモノアイアイコンを中央上1/3に表示

## このクラスに含まれるprivateメソッド

- `void StartFadeOut()` - MessageWindowUI::Hide()でメッセージ非表示化、BlackOutUI::FadeIn(kFadeDuration)で暗転開始、currentState_をState::FadingOutに設定

## このクラスで参照するアセットの情報

- BGM: `Asset/bgm_gameover.mp3` - ゲームオーバー時に流すホワイトノイズ

## このクラスが参照する仕様書の項目

- ゲームオーバーフェーズ
- フェーズ遷移図
- 演出

## このクラスが使用されるフェーズ

ゲームオーバーフェーズ

## 特記事項・メモ

- State列挙型の定義：
  - `ShowingMessage`: メッセージ表示中
  - `FadingOut`: 暗転中
  - `Finished`: 終了（暗転完了後はプレイヤーがウィンドウを閉じるのを待つ）
- ゲームオーバーメッセージ例（正気を失った表現）：
  - 「ア..ア......アアアアアアッァァァァァァァァァ」
  - 「もう...何も...」
  - 「終わりだ...全て...」
- 処理フロー:
  1. コンストラクタでSoundManager::PlayBGM("bgm_gameover")によりホワイトノイズBGM開始(ループ再生)
  2. MessageWindowUIに赤文字メッセージを設定し、テキスト色変更メソッドで赤色に設定
  3. メッセージ表示完了を待つ
  4. メッセージ完了後、BlackOutUI.FadeIn()で暗転開始
  5. 暗転完了後、State::Finishedへ遷移
  6. Finished状態では何もせず、プレイヤーがウィンドウを閉じるのを待つ(GameClearPhase、BadEndPhaseと同様)
- 赤文字表示はMessageWindowUIのテキスト色変更メソッドを使用する
- ホワイトノイズはループ再生で、フェーズ終了まで継続
- CommonUIは非表示
- 不穏で狂気的な雰囲気を演出するため、通常とは異なる視覚・聴覚効果を使用
