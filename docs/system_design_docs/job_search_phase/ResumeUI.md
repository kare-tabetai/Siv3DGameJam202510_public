# ResumeUI設計書

## 概要

履歴書の表示と入力を管理するUIクラス。基本情報・職歴・資格はオートコンプリート演出で自動入力され、自己PR/志望動機のみプレイヤーが入力できる。ログイン待機→オートコンプリート→入力待機の3状態を持つ。

## 目的・スコープ

現実の履歴書を模したUIを表示し、プレイヤーに自己PR/志望動機を入力させる。オートコンプリート演出により基本情報(国民番号、性別、年齢、住所、職制階級、賃金階級、学歴階級、不採用回数)、職歴、資格が一定間隔で自動的に入力される様子を表現し、AIによる管理社会の雰囲気を演出する。入力確定後、確認ダイアログを表示してからJobSearchPhaseに入力内容を渡す。

## ほかのクラスとの関係

- `JobSearchPhase` - このクラスから生成され、入力された自己PR/志望動機を渡す
- `SoundManager` - キーボード打鍵音とクリック音の再生を指示する
- `FontManager` - UI描画に使用するフォントを取得する

## このクラスが継承するクラス

なし

## このクラスのコンストラクタ

デフォルトコンストラクタを使用

## このクラスのデストラクタ

デフォルトデストラクタを使用

## このクラスに含まれるデータメンバ

- `String nationalID_` - 国民番号（オートコンプリートで設定される）
- `String gender_` - 生物学的性別（オートコンプリートで設定される）
- `String age_` - 年齢（オートコンプリートで設定される）
- `String address_` - 住所（オートコンプリートで設定される）
- `String wageRank_` - 賃金階級（オートコンプリートで設定される）
- `String jobRank_` - 雇用階級（オートコンプリートで設定される）
- `String educationalRank_` - 学歴階級（オートコンプリートで設定される）
- `String rejectionCount_` - 不採用回数（動的計算: kDefaultRejectionCount + 現在日数）
- `Array<String> workHistory_` - 職歴（オートコンプリートで配列に追加される）
- `Array<String> qualifications_` - 資格（オートコンプリートで配列に追加される）
- `mutable TextAreaEditState selfPREditState_` - 自己PR/志望動機の入力状態を管理するSiv3Dの編集状態オブジェクト
- `mutable String selfPRText_` - プレイヤーが入力した自己PR/志望動機のテキスト（現在未使用、selfPREditState_.textを参照）
- `mutable bool isConfirmed_` - 入力内容が確定されたかどうか
- `mutable ConfirmDialog confirmDialog_` - 確認ダイアログ（Show時に初期化される）
- `mutable bool enabled_` - UI入力の有効/無効フラグ（外部制御用）
- `mutable bool grayout_` - グレイアウト表示のフラグ（無効化時に半透明のオーバーレイを表示）
- `State state_` - 現在の状態（WaitLogIn/AutoComplete/WaitInput）
- `Stopwatch stopwatch_` - オートコンプリートのタイミング制御用
- `int autoCompleteIndex_` - オートコンプリート中の項目インデックス

## このクラスに含まれる定数

### 状態定義

- `enum class State` - UIの状態を表す列挙型
  - `WaitLogIn` - ログイン待機状態（初期状態）
  - `AutoComplete` - オートコンプリート演出中
  - `WaitInput` - プレイヤーの入力待機状態

### レイアウト関連

- `static constexpr double kResumeWidth` - 履歴書の幅（`GameConst::kPlayAreaSize.x * 5 / 6` = 1280 * 5 / 6 ≈ 1066.67px）
- `static constexpr double kResumeHeight` - 履歴書の高さ（`GameConst::kPlayAreaSize.y * 7 / 8` = 620 * 7 / 8 = 542.5px）
- `static constexpr Vec2 kResumeSize` - 履歴書のサイズ（{1066.67, 542.5}）
- `static constexpr Vec2 kResumeCenterPos` - 履歴書の中心位置（`GameConst::kPlayAreaCenter` = {640, 410}）
- `static constexpr Vec2 kResumeLeftTopPos` - 履歴書の左上位置（中心 - サイズ/2 = {106.67, 138.75}）
- `static constexpr RectF kResumeRect` - 履歴書の描画領域（{106.67, 138.75, 1066.67, 542.5}）
- `static constexpr double kCornerRadius = 0.0` - 角丸半径（角丸なし）
- `static constexpr double kWindowMargin = 20.0` - ウィンドウの余白
- `static constexpr double kAreaMargin = 10.0` - エリアの余白（追加）

### 基本情報エリア関連

- `static constexpr Vec2 kBasicInfoAreaPos` - 基本情報エリアの位置（`kResumeLeftTopPos + Vec2::One() * kWindowMargin` = {106.67, 138.75} + {20, 20} = {126.67, 158.75}）
- `static constexpr Vec2 kBasicInfoAreaSize` - 基本情報エリアのサイズ（`(kResumeSize - 3 * Vec2::One() * kWindowMargin) / 2` = ({1066.67, 542.5} - {60, 60}) / 2 = {503.34, 241.25}）
- `static constexpr ColorF kBasicInfoBorderColor{0.0, 0.0, 0.0}` - 基本情報エリアの枠線色

### 職歴・病歴・資格エリア関連

- `static constexpr Vec2 kWorkHistoryAreaPos` - 職歴・病歴・資格エリアの位置（`kBasicInfoAreaPos + Vec2{0, kBasicInfoAreaSize.y + kWindowMargin}` = {126.67, 158.75} + {0, 241.25 + 20} = {126.67, 420}）
- `static constexpr ColorF kWorkHistoryBorderColor{0.0, 0.0, 0.0}` - 職歴・病歴・資格エリアの枠線色

### 自己PRエリア関連

- `static constexpr Vec2 kSelfPRAreaPos` - 自己PRエリアの位置（`kBasicInfoAreaPos + Vec2{kWindowMargin + kBasicInfoAreaSize.x, 0}` = {126.67, 158.75} + {20 + 503.34, 0} = {650, 158.75}）
- `static constexpr Vec2 kSelfPRAreaSize` - 自己PRエリアのサイズ（`{kBasicInfoAreaSize.x, kResumeSize.y - kWindowMargin * 2}` = {503.34, 542.5 - 40} = {503.34, 502.5}）
- `static constexpr Vec2 kSubmitButtonSize{100, 40}` - 決定ボタンのサイズ（追加）
- `static constexpr ColorF kSelfPRBorderColor{0.0, 0.0, 0.0}` - 自己PRエリアの枠線色

### フォント関連

- `static constexpr int32 kHeaderFontSize = 32` - 見出しのフォントサイズ
- `static constexpr int32 kBodyFontSize = 20` - 本文のフォントサイズ
- `static constexpr int32 kSmallFontSize = 16` - 小さい文字のフォントサイズ

### 色関連

- `static constexpr ColorF kTextColor = UiConst::kUITextColor` - テキストの色（UiConstから取得）
- `static constexpr ColorF kResumeBackgroundColor = UiConst::kWindowColor` - 履歴書ウィンドウの背景色（UiConstから取得）
- `static constexpr ColorF kWindowBorderColor = UiConst::kWindowBorderColor` - ウィンドウ枠の色（UiConstから取得）
- `static constexpr double kWindowBorderThickness = UiConst::kWindowBorderThickness` - ウィンドウ枠の太さ（UiConstから取得）
- `static constexpr ColorF kInputAreaColor = UiConst::kInputAreaColor` - 入力エリアの背景色（UiConstから取得）
- `static constexpr ColorF kDisabledOverlayColor{0.0, 0.0, 0.0, 0.5}` - 無効化時のオーバーレイ色（黒：半透明）

### その他

- `static constexpr int32 kMaxSelfPRLength = 100` - 自己PR/志望動機の最大文字数（80から100に変更）
- `static constexpr double kAutoCompleteInterval = 0.4` - オートコンプリートの入力間隔（秒）

### 固定データ

- `static constexpr int kDefaultRejectionCount = 81` - ゲームが始まるまでに不採用になっていた回数
- `static inline const String kNationalID = U"KU1623142"` - 国民番号
- `static inline const String kGender = U"男"` - 生物学的性別
- `static inline const String kAge = U"26歳"` - 年齢
- `static inline const String kAddress = U"千葉県船橋市日の出区平井3123-456-789"` - 住所
- `static inline const String kJobRank = U"E"` - 雇用階級
- `static inline const String kWageRank = U"D"` - 賃金階級
- `static inline const String kEducationalRank = U"A"` - 学歴階級
- `static inline const Array<String> kWorkHistory` - 職歴（配列）
  - `U"2029年3月 横浜工業大学 工学研究科 修士課程終了\n"`
  - `U"2029年4月〜2030年1月 サイバーフューチャー株式会社 正社員\n"`
  - `U"2030年10月〜2031年3月 芙蓉テクノホールディングス 正社員"`
- `static inline const Array<String> kQualifications` - 資格（配列）
  - `U"普通自動車免許(AI限)\n"`
  - `U"高等学校教諭一種免許状\n"`
  - `U"応用情報技術者"`

## このクラスに含まれるpublicメソッド

- `void Show()` - 履歴書UIを表示開始する。内部状態をリセット（isConfirmed_, selfPRText_, selfPREditState_, confirmDialog_をクリア）し、オートコンプリートのインデックスとタイマーをリセットする
- `void Update()` - 毎フレーム呼ばれる更新処理。確認ダイアログの更新、状態に応じた処理(WaitLogIn/AutoComplete/WaitInput)を行う
- `void Draw() const` - 毎フレーム呼ばれる描画処理。履歴書UIを描画し、grayout_がtrueの場合は無効化オーバーレイを表示し、確認ダイアログを描画する
- `[[nodiscard]] bool IsConfirmed() const noexcept` - 入力が確定されたかどうかを返す
- `[[nodiscard]] String GetSelfPR() const` - 入力された自己PR/志望動機のテキストを取得する（selfPREditState_.textを返す）
- `void SetInputEnabled(bool enabled) noexcept` - テキストエリアと送信ボタンの有効/無効を設定する
- `[[nodiscard]] bool IsEnabled() const noexcept` - 現在の有効/無効状態を返す
- `[[nodiscard]] State GetState() const noexcept` - 現在の状態を返す
- `void SetGrayOut(bool grayout) noexcept` - グレイアウト表示の有効/無効を設定する
- `void StartAutoComplete()` - オートコンプリート演出を開始する（状態がWaitLogInの場合のみ）。autoCompleteIndex_を0にリセットし、stopwatch_を再起動する

## このクラスに含まれるprivateメソッド

- `void DrawResumeBackground() const` - 履歴書の背景矩形を描画する（kResumeRectを使用、角丸なし、白背景、黒枠2ピクセル）
- `void DrawBasicInfo() const` - 基本情報欄を描画する（左上のボックス）。国民番号・性別・年齢・住所・職制階級・賃金階級・学歴階級・不採用回数を表示
- `void DrawWorkHistory() const` - 職歴・資格欄を描画する（左下のボックス）。職歴と資格を表示
- `void DrawSelfPRArea() const` - 自己PR欄を描画する（右側のボックス）。見出し、テキストエリア、文字数カウンター、送信ボタンを表示する。ボタンクリック時に確認ダイアログを表示
- `void PlayTypingSound()` - キーボード打鍵音をランダムに再生する（3種類から選択）。音量は0.3で控えめに再生
- `[[nodiscard]] String FormatDate(int32 date) const` - 日付をフォーマットする（例: 20310622 -> "2031/06/22"）
- `void UpdateAutoComplete()` - オートコンプリート演出の更新処理。kAutoCompleteInterval間隔で項目を補完し、autoCompleteIndex_をインクリメント。全項目完了でWaitInputに遷移
- `static const String CountRejectionCount()` - 不採用回数を計算する（kDefaultRejectionCount + GameCommonData::GetCurrentDay()）
- `[[nodiscard]] bool ControlsEnabled() const noexcept` - enabled_と確認ダイアログの表示状態を組み合わせて実際に操作可能かを返す

## このクラスで参照するアセットの情報

- フォント: `resume_medium` - Source Han Serifの中サイズフォント（FontManagerから取得）
- フォント: `resume_small` - Source Han Serifの小サイズフォント（FontManagerから取得）
- フォント: `ui_medium` - IBM Plex Sans JPの中サイズフォント（FontManagerから取得）
- SE: `se_keyboard1`, `se_keyboard2`, `se_keyboard3` - キーボード打鍵音（SoundManagerから再生）
- SE: `se_click` - マウスクリック音（SoundManagerから再生）

## このクラスが参照する仕様書の項目

- 求職活動フェーズ
- 履歴書UI
- 主人公の経歴

## イメージ図

![履歴書UIのイメージ図](../spec_image/resume_ui.png)

## このクラスが使用されるフェーズ

- 求職活動フェーズ

## 特記事項・メモ

- このクラスはイベント駆動型の設計で、外部から`Show()`で初期化して、毎フレーム`Update()`と`Draw()`を呼び出す
- 3つの状態(WaitLogIn/AutoComplete/WaitInput)を持ち、StartAutoComplete()で状態遷移を開始
- 履歴書のレイアウトは左右2分割で構成
  - 左側: 基本情報（上）と職歴・資格（下）
  - 右側: 自己PR（プレイヤー入力部分）
- オートコンプリート演出は一定間隔(0.4秒)で項目単位で表示される
  - 順序: 基本情報8項目（国民番号、性別、年齢、住所、職制階級、賃金階級、学歴階級、不採用回数） → 職歴3項目 → 資格3項目
- 自己PR入力エリアはSiv3Dの`SimpleGUI::TextArea`を使用（最大100文字）
- 文字数カウンターをテキストエリアの下に表示
- 確認ダイアログのボタン判定は毎フレーム行われ、「はい」がクリックされると`isConfirmed_`フラグが立つ
- レイアウト計算は`GameConst::kPlayAreaSize`と`GameConst::kPlayAreaCenter`に基づいて動的に計算される
- 色定義は`UiConst`から取得
- UIの角丸は無し（`kCornerRadius = 0.0`）で冷たさを演出
- 各エリアは黒い枠線（2ピクセル）で区切られている
- `grayout_`がtrueのときは半透明の黒オーバーレイで無効化表示
- `enabled_`と確認ダイアログの表示状態を組み合わせて実際の操作可否を判定
- 不採用回数は動的に計算される（デフォルト値81 + 現在の日数）
