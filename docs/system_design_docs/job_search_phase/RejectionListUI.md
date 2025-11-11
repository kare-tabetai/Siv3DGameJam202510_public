# RejectionListUI設計書

## 概要

不採用通知のリストを表示するUIクラス。これまでに受けた全ての不採用情報を履歴書形式で一覧表示し、プレイヤーに「社会から必要とされていない」という精神的苦痛を与える演出を担当する。

## 目的・スコープ

求職活動で不採用となった企業の情報(企業名とお祈りメッセージ)をリスト形式で表示する。履歴書UIと同じサイズで表示し、複数の不採用情報が縦に並ぶレイアウトを実現する。プレイヤーに主人公の孤独感と絶望感を追体験させることが目的。

## ほかのクラスとの関係

- `JobSearchPhase` - このクラスから生成され、Show()/RecruitShow()で表示指示を受ける
- `RejectionInfo` - 不採用情報を構造化したデータを配列として保持し、Draw()メソッドで描画を委譲
- `ConfirmDialog` - 全件表示後に「はい」のみのダイアログを表示
- `SoundManager` - クリック音の再生を指示する（ConfirmDialog経由）
- `FontManager` - フォントの取得に使用（RejectionInfo経由）
- `UiConst` - UI定数の参照に使用
- `GameConst` - ゲーム定数の参照に使用

## このクラスが継承するクラス

なし

## このクラスのコンストラクタ

デフォルトコンストラクタを使用

## このクラスのデストラクタ

デフォルトデストラクタを使用

## このクラスに含まれるデータメンバ

- `Array<RejectionInfo> rejections_` - 不採用情報のリスト（初期値空配列）
- `bool isVisible_ = false` - UIが表示中かどうか（初期値false）
- `State state_ = State::InitialLoading` - UIの状態（初期値InitialLoading）
- `size_t currentRejectionIndex_ = 0` - 現在表示中の不採用情報インデックス（初期値0）
- `Type type_ = Type::Rejection` - 表示タイプ（初期値Rejection）
- `ConfirmDialog confirmDialog_` - 確認ダイアログ（初期値デフォルトコンストラクタ）
- `Stopwatch stopwatch_` - 演出用タイマー（初期値未定義）

### 内部列挙型

#### State enum
- `InitialLoading` - リストをひとつづつ表示する演出中
- `DialogInterval` - リストがすべて表示されてから、ダイアログが表示されるまでの待機
- `ShowingDialog` - ダイアログ表示中

#### Type enum
- `Rejection` - 不採用モード
- `Recruitment` - 採用モード

## このクラスに含まれる定数

### ウィンドウレイアウト定数
- `static constexpr double kListSizeWidth = GameConst::kPlayAreaSize.x * 5 / 6` - 不採用リストの幅
- `static constexpr double kListSizeHeight = GameConst::kPlayAreaSize.y * 7 / 8` - 不採用リストの高さ
- `static constexpr double kWindowMargin = 20.0` - ウィンドウの余白
- `static constexpr Vec2 kListSize = {kListSizeWidth, kListSizeHeight}` - 不採用リストのサイズ
- `static constexpr Vec2 kListCenterPos = GameConst::kPlayAreaCenter` - 中央位置
- `static constexpr Vec2 kListTopLeftPos = kListCenterPos - kListSize / 2` - 不採用リストの表示位置

### 不採用情報表示エリア定数
- `static constexpr Vec2 kRejectionInfoAreaTopLeft = kListTopLeftPos + Vec2{kWindowMargin, kWindowMargin}` - 不採用情報表示エリア左上
- `static constexpr double kRejectionInfoAreaWidth = kListSize.x - kWindowMargin * 2` - 不採用情報エリアの幅
- `static constexpr double kRejectionInfoAreaHeight = kListSize.y - kWindowMargin * 2` - 不採用情報エリアの高さ
- `static constexpr Vec2 kRejectionInfoAreaSize = {kRejectionInfoAreaWidth, kRejectionInfoAreaHeight}` - 不採用情報エリアのサイズ

### ボタン定数
- `static constexpr Vec2 kCloseButtonCenterPos = {kListCenterPos.x, kListTopLeftPos.y + kListSize.y + kWindowMargin}` - 閉じるボタン中心座標
- `static constexpr Size kCloseButtonSize = {100, 40}` - 閉じるボタンサイズ

### アイテム表示定数
- `static constexpr size_t kRejectionInfoCount = 10` - 不採用情報の最大表示数
- `static constexpr double kRejectionInfoHeight = kRejectionInfoAreaHeight / kRejectionInfoCount` - 1項目当たりの高さ
- `static constexpr double kItemHeight = 48.0` - 1つの不採用情報の高さ(参考値)
- `static constexpr double kItemSpacing = 6.0` - 不採用情報間の余白(罫線のみ)
- `static constexpr double kListPaddingTop = 44.0` - リスト上部の余白
- `static constexpr double kListPaddingLeft = 60.0` - リスト左側の余白
- `static constexpr double kListPaddingRight = 60.0` - リスト右側の余白

### 演出タイミング定数
- `static constexpr double kItemDisplayInterval = 0.18` - 不採用情報を一件ずつ表示する間隔(秒)
- `static constexpr double kDialogIntervalDuration = 0.8` - 全件表示後ダイアログを出すまでの待機時間(秒)

### 色・枠線定数
- `static constexpr ColorF kBackgroundColor = UiConst::kInputAreaColor` - リスト内背景色(白系)
- `static constexpr ColorF kOuterBackgroundColor = UiConst::kWindowColor` - リスト外側の背景は共通ウィンドウ色
- `static constexpr ColorF kBorderColor{0.0, 0.0, 0.0}` - 枠線の色(黒)
- `static constexpr double kBorderThickness = 1.0` - 枠線の太さ
- `static constexpr double kCornerRadius = 0.0` - 角丸半径(角丸なし)

### メッセージ定数
- `static inline const String kDialogMessage` - 不採用時のダイアログメッセージ（"採用判定件数0/100\n本日の採用結果は「全社不採用」となりました。\n明日のご応募をお待ちしています"）
- `static inline const String kRecruiteDialogMessage` - 採用時のダイアログメッセージ（"採用判定件数100/100\nおめでとうございます!採用が決定しました。\n後日送付される採用通知をご確認ください。"）

## このクラスに含まれるpublicメソッド

- `void Show()` - isVisible_=true、SetUpregections()でrejections_初期化、state_=State::InitialLoading、currentRejectionIndex_=0、stopwatch_.reset()とstopwatch_.restart()でタイマー初期化、confirmDialog_.Hide()でダイアログ非表示、type_=Type::Rejectionで不採用モード設定
- `void RecruitShow()` - isVisible_=true、SetUpRecruitments()でrejections_を採用情報で初期化、state_=State::InitialLoading、currentRejectionIndex_=0、stopwatch_.reset()とstopwatch_.restart()、confirmDialog_.Hide()、type_=Type::Recruitmentで採用モード設定
- `void Update()` - isVisible_がfalseなら早期リターン。stateに応じてUpdateInitialLoading()/UpdateDialogInterval()/UpdateShowingDialog()を呼び出し
- `void Draw() const` - isVisible_がfalseなら早期リターン。listRect.rounded(kCornerRadius).draw(kBackgroundColor).drawFrame(kBorderThickness, kBorderColor)でリスト背景描画。Min(currentRejectionIndex_, rejections_.size())までループしてrejections_[i].Draw(itemRect)で各不採用情報描画。state_==State::ShowingDialogならconfirmDialog_.Draw()でダイアログ上書き描画
- `[[nodiscard]] bool IsVisible() const noexcept` - isVisible_を返す
- `[[nodiscard]] bool IsClosed() const noexcept` - !isVisible_を返す
- `void Reset()` - rejections_.clear()、isVisible_=false、confirmDialog_.Hide()で状態リセット


## このクラスに含まれるprivateメソッド

- `void SetUpregections()` - rejections_.clear()、kRejectionInfoCount回ループしてRejectionInfo rejection生成、rejections_.push_back(rejection)で不採用情報リストを初期化
- `void SetUpRecruitments()` - rejections_.clear()、kRejectionInfoCount回ループしてRejectionInfo::CreateRecruitmentInfo()で採用情報生成、rejections_.push_back(recruitment)で採用情報リストを初期化
- `void UpdateInitialLoading()` - stopwatch_.sF() >= kItemDisplayIntervalでcurrentRejectionIndex_ < rejections_.size()なら++currentRejectionIndex_して次のアイテム表示、stopwatch_.restart()でタイマー再開。currentRejectionIndex_ >= rejections_.size()ならstate_=State::DialogInterval、stopwatch_.restart()で次状態へ遷移
- `void UpdateDialogInterval()` - stopwatch_.sF() >= kDialogIntervalDurationでstate_=State::ShowingDialog、stopwatch_.reset()、dialogMessage=(type_==Type::Recruitment)?kRecruiteDialogMessage:kDialogMessageで適切なメッセージ選択、confirmDialog_.Show(dialogMessage, kListCenterPos, true, 14)で「はい」のみダイアログ表示
- `void UpdateShowingDialog()` - confirmDialog_.Update()でダイアログ更新、confirmDialog_.IsConfirmed()ならisVisible_=falseで閉じる

## このクラスで参照するアセットの情報

- フォント: `Asset/IBM_Plex_Sans_JP/IBMPlexSansJP-Regular.ttf` - IBM Plex Sans JPフォント
- SE: `Asset/PC-Mouse05-1.mp3` - マウスクリック音

## このクラスが参照する仕様書の項目

- 求職活動フェーズ
- 不採用リストUI

## イメージ図

![不採用リストUIのイメージ図](../spec_image/rejection_list_ui.png)

## このクラスが使用されるフェーズ

- 求職活動フェーズ

## 特記事項・メモ

- 不採用リストUIは履歴書UIと同じサイズで中央に表示する
- UIの角は冷たさを演出するため角丸にしない(kCornerRadius = 0.0)
- 個別の不採用情報の描画は`RejectionInfo`クラスの`Draw`メソッドに委譲する
- フォントサイズや色などの描画に関連する定数は`RejectionInfo`クラスで管理する
- 3つの状態遷移: InitialLoading（一件ずつ表示演出、0.18秒間隔）→ DialogInterval（0.8秒待機）→ ShowingDialog（「はい」のみのダイアログ表示）
- Type enumでRejection/Recruitmentを切り替え、採用時は全件「採用」情報を表示し、採用メッセージダイアログを表示
- RecruitShow()は採用モードでの表示、Show()は不採用モードでの表示
- ダイアログはshowOnlyYes=trueで「はい」ボタンのみ表示、messageFontSize=14で小さめのフォントサイズ指定
- スクロール機能は実装されていない（最大10件の固定表示）
