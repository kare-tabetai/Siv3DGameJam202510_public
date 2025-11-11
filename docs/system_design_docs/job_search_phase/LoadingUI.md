# LoadingUI設計書

## 概要

LLMによる履歴書評価の待ち時間中に表示されるローディング画面UIクラス。世界観を説明する広告文章を8秒ごとに切り替えて表示し、プレイヤーの待ち時間を軽減しつつ世界観の没入感を高める。

## 目的・スコープ

ローカルLLMによる評価処理の待ち時間をプレイヤーに感じさせないよう、世界観に沿った広告文章を表示する。ロード中であることを示すアニメーションと、AIによる管理社会の雰囲気を伝える広告文章の両方を表示する。

## ほかのクラスとの関係

- `JobSearchPhase` - このクラスから生成され、表示/非表示の指示を受ける
- `SoundManager` - 必要に応じて効果音の再生を指示する
- `RotatingIcon` - 回転するアイコンの描画を管理する（LoadingUIから委譲）
- `ServerLoadingUI` - サーバーローディングUI（メンバとして保持、現在は未使用の可能性あり）
- `FontManager` - フォントの取得に使用
- `UiConst` - UI定数の参照に使用

## このクラスが継承するクラス

なし

## このクラスのコンストラクタ

デフォルトコンストラクタを使用

## このクラスのデストラクタ

デフォルトデストラクタを使用

## このクラスに含まれるデータメンバ

- `int32 currentAdIndex_ = 0` - 現在表示中の広告のインデックス（初期値0）
- `Timer adChangeTimer_` - 広告切り替え用のタイマー（初期値未定義）
- `bool isVisible_ = false` - UIが表示中かどうか（初期値false）
- `RotatingIcon rotatingIcon_` - 回転するアイコンの管理クラス（初期値デフォルトコンストラクタ）
- `Texture logoTexture_` - 下部ロゴのテクスチャ（LoadingUI側で管理）
- `ServerLoadingUI serverLoadingUI_` - サーバーローディングUI（初期値デフォルトコンストラクタ）

## このクラスに含まれる定数

- `static constexpr double kAdChangeInterval = 8.0` - 広告切り替え間隔(秒)
- `static constexpr Vec2 kDialogSize{1100, 425}` - ダイアログのサイズ
- `static constexpr Vec2 kDialogPos{90, 205}` - ダイアログの表示位置(画面左上からの絶対位置)
- `static constexpr double kDialogRadius = UiConst::kUiWindowRadius` - ダイアログの角丸半径
- `static constexpr int32 kAdFontSize = 24` - 広告文章のフォントサイズ
- `static constexpr int32 kLoadingFontSize = 32` - 「評価中」のフォントサイズ
- `static constexpr double kIconSize = 60.0` - ローディングアイコンのサイズ(六角形の外接円の直径)
- `static constexpr double kBottomLogoSize = 80.0` - 下部ロゴの表示サイズ（ピクセル）
- `static constexpr Vec2 kBottomLogoOffset{40, 325}` - ダイアログ左上からの下部ロゴオフセット
- `static constexpr ColorF kTextColor = UiConst::kUITextColor` - テキストの色
- `static constexpr ColorF kBackgroundColor = UiConst::kWindowColor` - 背景色
- `static constexpr Vec2 kLoadingTextPos{640, 318}` - 「評価中」テキストの表示位置
- `static constexpr Vec2 kIconPos{640, 380}` - ローディングアイコンの表示位置
- `static constexpr Vec2 kAdTextPos{240, 525}` - 広告文章の表示位置
- `static constexpr double kAdTextWidth = 900.0` - 広告文章の表示幅
- `static inline const String kLoadingLogoPath = String(UiConst::kMonoEyeIconPath)` - ロゴのアセットパス（UiConstから取得）
- `static inline const Array<String> kAdvertisements` - 表示する広告文章のリスト（5つの広告文）

## このクラスに含まれるpublicメソッド

- `void Show()` - isVisible_=true、currentAdIndex_=Random(0, kAdvertisements.size()-1)でランダムな初期広告選択、adChangeTimer_=Timer{SecondsF{kAdChangeInterval}, StartImmediately::Yes}で広告タイマー開始、rotatingIcon_.Initialize(kIconSize, kIconPos)でアイコン初期化、rotatingIcon_.Show()で表示開始、logoTexture_=Texture{kLoadingLogoPath}でロゴ読み込み
- `void Hide()` - isVisible_=falseで非表示にする
- `void Update()` - isVisible_がfalseなら早期リターン。UpdateAdvertisement()で広告更新、rotatingIcon_.Update()でアイコン回転更新
- `void Draw() const` - isVisible_がfalseなら早期リターン。DrawDialog()でダイアログ描画、rotatingIcon_.Draw()でアイコン描画、DrawLoadingIcon()でロゴ描画、DrawAdvertisement()で広告文描画
- `[[nodiscard]] bool IsVisible() const noexcept` - isVisible_を返す

## このクラスに含まれるprivateメソッド

- `void UpdateAdvertisement()` - adChangeTimer_.reachedZero()でcurrentAdIndex_=(currentAdIndex_+1)%kAdvertisements.size()で次の広告に切り替え、adChangeTimer_.restart()でタイマー再開
- `void DrawDialog() const` - dialogRect.rounded(kDialogRadius).draw(kBackgroundColor).drawFrame(UiConst::kWindowBorderThickness, UiConst::kWindowBorderColor)でダイアログ描画、font(U"評価中").drawAt(kLoadingTextPos, kTextColor)で「評価中」テキスト描画
- `void DrawAdvertisement() const` - currentAdIndex_ >= kAdvertisements.size()なら早期リターン。font(kAdvertisements[currentAdIndex_]).draw(kAdFontSize, kAdTextPos, kTextColor)で現在の広告文章を描画
- `void DrawLoadingIcon() const` - logoTexture_が有効ならlogoTexture_.resized(kBottomLogoSize).draw(kDialogPos + kBottomLogoOffset)で下部ロゴを描画

## このクラスで参照するアセットの情報

- フォント: `Asset/IBM_Plex_Sans_JP/IBMPlexSansJP-Regular.ttf` - IBM Plex Sans JPフォント
- 画像(ロゴ): `UiConst::kMonoEyeIconPath` により指定されるロゴ画像（プロジェクト共通定義を使用）
- 画像(アイコン): `Asset/loading_icon.png` - ロード中アイコン画像
 - フォント: `FontManager::GetFont("ui_large")` / `FontManager::GetFont("ui_medium")` を利用して描画する（ヘッダは `FontManager` を参照）


## このクラスが参照する仕様書の項目

- 求職活動フェーズ
- ロード中ダイアログ

## イメージ図

![ロード中UIのイメージ図](../spec_image/loading_ui.png)

## このクラスが使用されるフェーズ

- 求職活動フェーズ

## 特記事項・メモ

- 広告文章の例（kAdvertisementsに以下のような文章を定数として定義）:
  - "モノアイを介さない企業の採用活動(闇採用)は、違法行為です!!"
  - "モノアイ最新版にて、国民番号データベースとの連携機能を追加しました。データベースの情報をもとに面倒だった入力項目を補完できます。"
  - "プロンプトインジェクションを用いたモノアイの利用は、犯罪です。法により５年以下の懲役、または1000万円以下の罰金が科せられることがあります。"
- 広告文章リストの初期化方法:

  ```cpp
  static inline const Array<String> kAdvertisements = {
      U"モノアイを介さない企業の採用活動(闇採用)は、違法行為です!!",
      U"モノアイ最新版にて、国民番号データベースとの連携機能を追加しました。データベースの情報をもとに面倒だった入力項目を補完できます。",
      U"プロンプトインジェクションを用いたモノアイの利用は、犯罪です。法により５年以下の懲役、または1000万円以下の罰金が科せられることがあります。"
  };
  ```
  
- ヘッダファイル内で直接初期化するため、別途初期化メソッドは不要
- ローディングアイコンは円形で、回転アニメーションにより「処理中」であることを視覚的に示す
- 広告文章は8秒ごとに自動的に次の文章へ切り替わる
- ダイアログは画面中央に表示し、背景を暗くすることでプレイヤーの注意を引く
- LLMの処理が完了したら自動的に非表示になる
- ヘッダ実装では広告テキストの一覧に複数の文言（警告文や世界観を伝える文）を直接定義しており、Show() 時にランダムな初期インデックスが選ばれ、8秒ごとに次の文章へ切り替わります。
- ローディングアイコンの回転は `rotationAngle_` を `Scene::DeltaTime()` に基づいて更新し、サイン波で速度を変調する（`kRotationModulationAmplitude` と `kRotationModulationPeriod` を利用）。
- 描画時にテクスチャが読み込まれていない場合は、代替としてスケールを反映した 円（Circle）を描画する実装になっている。
- ローディングアイコンのスケールは `elapsedTime_` をもとにサイン波で 1.0 ± `kScaleModulationAmplitude` の範囲で変化する。