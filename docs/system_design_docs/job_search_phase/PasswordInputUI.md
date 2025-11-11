# PasswordInputUI設計書

## 概要

求職活動フェーズの冒頭で表示される、モノアイサーバーへのパスワード入力演出を行うUIクラス。実際の操作はできず、自動でパスワードが入力されていく様子を演出する。

## 目的・スコープ

プレイヤーが操作することなく、自動的にパスワードが入力される演出を表示する。この演出により、AIシステム「モノアイ」にアクセスしている雰囲気を醸成する。実際の入力機能は持たず、視覚的な演出のみを担当する。

## ほかのクラスとの関係

- `JobSearchPhase` - このクラスから生成され、演出の進行状態を報告する
- `SoundManager` - キーボード打鍵音の再生を指示する

## このクラスが継承するクラス

なし

## このクラスのコンストラクタ

デフォルトコンストラクタを使用

## このクラスのデストラクタ

デフォルトデストラクタを使用

## このクラスに含まれるデータメンバ

- `String displayPassword_` - 現在表示されているパスワード文字列（初期値空文字列）
- `Timer inputTimer_` - 文字入力のタイミングを制御するタイマー（初期値未定義）
- `Timer stateTimer_` - 状態遷移のタイミングを制御するタイマー（Display→PasswordInput遷移用）
- `Timer waitTimer_` - 入力完了後の短い待機を制御するタイマー（InputWait→Completed遷移用）
- `size_t currentCharIndex_ = 0` - 現在入力中の文字インデックス（初期値0）
- `State currentState_ = State::Display` - UIの現在状態（初期値State::Display）
- `Texture iconTexture_` - モノアイアイコンのテクスチャ（コンストラクタでkMonoEyeIconPathから読み込み）


## このクラスに含まれる定数

- `static inline const String kPasswordText = U"●●●●●●●●●●"` - 表示するパスワード（10文字の黒丸）
- `static constexpr Vec2 kWindowPos{260, 160}` - ウィンドウ左上位置
- `static constexpr Size kWindowSize{800, 420}` - ウィンドウサイズ
- `static constexpr Vec2 kWindowCenter = kWindowPos + kWindowSize / 2` - ウィンドウ中心位置
- `static constexpr double kCharInputInterval = 0.25` - 1文字あたりの入力間隔(秒)
- `static constexpr double kDisplayDuration = 1.8` - 表示フェーズの表示時間(秒)
- `static constexpr double kInputWaitDuration = 1.5` - 入力完了後の待機時間(秒)
- `static constexpr Vec2 kIconPos = {kWindowCenter.x, kWindowPos.y + kWindowSize.y / 3}` - 六角形アイコンの表示位置
- `static constexpr Vec2 kCitizenNumberLabelPos{420, 450}` - 「国民番号:」ラベルの表示位置
- `static constexpr Vec2 kCitizenNumberInputPos{570, 440}` - 国民番号入力欄の表示位置
- `static constexpr Vec2 kPasswordLabelPos{390, 520}` - 「パスワード:」ラベルの表示位置
- `static constexpr Vec2 kPasswordInputPos{570, 510}` - パスワード入力欄の表示位置
- `static constexpr double kInputTextPadding = 3.0` - 入力欄内のテキストのパディング
- `static inline const String kCitizenNumber = U"KU1623142"` - 表示する国民番号(固定値)
- `static constexpr Size kInputBoxSize{400, 50}` - 入力欄のサイズ
- `static constexpr int32 kLabelFontSize = 28` - ラベル表示のフォントサイズ
- `static constexpr int32 kInputFontSize = 32` - 入力欄のフォントサイズ
- `static constexpr ColorF kTextColor = UiConst::kUITextColor` - テキストの色
- `static constexpr ColorF kInputBoxColor = UiConst::kInputAreaColor` - 入力欄の背景色
- `static constexpr ColorF kInputBoxBorderColor = UiConst::kInputAreaBorderColor` - 入力欄の枠線色
- `static constexpr double kInputBoxBorderThickness = UiConst::kInputAreaThickness` - 入力欄の枠線の太さ
- `static constexpr double kInputBoxRadius = UiConst::kInputAreaRadius` - 入力欄の角丸半径
- `static constexpr double kIconSize = 180.0` - 六角形アイコンのサイズ
- `static constexpr StringView kMonoEyeIconPath = UiConst::kMonoEyeIconPath` - モノアイアイコンのアセットパス
- `static constexpr ColorF kWindowColor = UiConst::kWindowColor` - ウィンドウ背景色
- `static constexpr ColorF kWindowBorderColor = UiConst::kWindowBorderColor` - ウィンドウ枠の色
- `static constexpr double kWindowBorderThickness = UiConst::kWindowBorderThickness` - ウィンドウ枠の太さ
- `static constexpr double kWindowRadius = UiConst::kUiWindowRadius` - ウィンドウの角丸半径


## このクラスに含まれるpublicメソッド

- `void Show()` - displayPassword_.clear()、currentCharIndex_=0、currentState_=State::Display、stateTimer_=Timer{SecondsF(kDisplayDuration), StartImmediately::Yes}、inputTimer_=Timer{SecondsF(kCharInputInterval)}、waitTimer_=Timer{}で演出状態を初期化
- `void Update()` - currentState_に応じてUpdateDisplayPhase()/UpdatePasswordInputPhase()/UpdateInputWaitPhase()を呼び出し（Completedでは何もしない）
- `void Draw() const` - windowRect.rounded(kWindowRadius).draw(kWindowColor).drawFrame()でウィンドウ描画、iconTexture_.resized(kIconSize).draw(Arg::center = kIconPos)でアイコン描画、citizenNumberBox/passwordBoxでrounded().draw().drawFrame()で入力欄描画、inputFont(kCitizenNumber).draw()とinputFont(displayPassword_).draw()で入力内容描画、labelFont(U"国民番号:")/labelFont(U"パスワード:")でラベル描画
- `[[nodiscard]] bool IsCompleted() const noexcept` - currentState_ == State::Completedを返す
- `[[nodiscard]] State GetState() const noexcept` - currentState_を返す

## このクラスに含まれるprivateメソッド

- `void UpdateDisplayPhase()` - stateTimer_.reachedZero()でcurrentState_=State::PasswordInput、inputTimer_=Timer{SecondsF(kCharInputInterval), StartImmediately::Yes}でパスワード入力フェーズへ遷移
- `void UpdatePasswordInputPhase()` - inputTimer_.reachedZero()でInputNextChar()呼び出し。currentCharIndex_ >= kPasswordText.size()でcurrentState_=State::InputWait、waitTimer_=Timer{SecondsF(kInputWaitDuration), StartImmediately::Yes}でInputWaitフェーズへ遷移。それ以外はinputTimer_.restart()で次の文字入力準備
- `void UpdateInputWaitPhase()` - waitTimer_.isStarted() && waitTimer_.reachedZero()でcurrentState_=State::Completed、SoundManager::PlaySE(U"se_keyboard4")でEnter音再生
- `void InputNextChar()` - currentCharIndex_ < kPasswordText.size()でdisplayPassword_ += kPasswordText[currentCharIndex_]、currentCharIndex_++、PlayTypingSound()で次の文字を入力して効果音再生
- `void PlayTypingSound()` - typingSounds = {U"se_keyboard1", U"se_keyboard2", U"se_keyboard3"}からRandom<size_t>(0, typingSounds.size() - 1)でランダム選択、SoundManager::PlaySE(selectedSound)でキーボード打鍵音をランダムに再生

## このクラスで参照するアセットの情報

- フォント: `Asset/IBM_Plex_Sans_JP/IBMPlexSansJP-Regular.ttf` - IBM Plex Sans JPフォント
- SE: `Asset/keyboard/タイピング-メカニカル単1.mp3` - キーボード打鍵音1
- SE: `Asset/keyboard/タイピング-メカニカル単2.mp3` - キーボード打鍵音2
- SE: `Asset/keyboard/タイピング-メカニカル単3.mp3` - キーボード打鍵音3
- 背景画像: `Asset/background.png` - UI背景画像

## このクラスが参照する仕様書の項目

- 求職活動フェーズ
- パスワード入力UI

## イメージ図

![パスワード入力UIのイメージ図](spec_image/passward_input_ui.png)

## このクラスが使用されるフェーズ

- 求職活動フェーズ

## 特記事項・メモ

- 操作不可能な演出専用のUIであり、プレイヤーからの入力は一切受け付けない
- タイピング音は3種類の中からランダムに選んで再生することで、リアリティを演出する
- 演出完了後、自動的に次のUI(ResumeUI)へ遷移する
- `isCompleted_` フラグは廃止し、代わりに `enum class State { Display, PasswordInput, InputWait, Completed }` を導入しました。
- 表示フェーズ（Display）→（kDisplayDuration 秒経過）→パスワード入力演出フェーズ（PasswordInput）→（全文字入力完了）→入力待機フェーズ（InputWait）→（kInputWaitDuration 秒経過）→終了（Completed）という遷移を実装済みです。
- 実装に合わせ、`kDisplayDuration`（表示フェーズの長さ）と `stateTimer_` を追加しています。
