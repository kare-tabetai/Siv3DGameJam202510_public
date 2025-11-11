# RejectionInfo設計書

## 概要

1つの企業からの不採用情報を保持し、描画も行うクラス。企業名、お祈りメッセージ、採用結果をまとめて管理し、指定された領域に描画する。

## 目的・スコープ

不採用リストUIで表示する個別の不採用情報を表現するデータ構造。企業ごとの不採用情報を構造化して保持し、RejectionListUIでの表示に使用される。データの保持に加えて、指定された矩形領域への描画機能も提供する。

## ほかのクラスとの関係

- `RejectionListUI` - このクラスのインスタンスの配列を保持し、不採用リストとして表示する。描画時にはこのクラスのDrawメソッドを呼び出す
- `JobSearchPhase` - 新しい不採用情報を生成する際に使用する
- `FontManager` - 描画に使用するフォントを取得する
- `UiConst` - UI共通の定数を参照する

## このクラスが継承するクラス

なし

## このクラスのコンストラクタ

デフォルトコンストラクタ - companyName=kCompanyNames[Random(0, kCompanyNames.size()-1)]、message=kRejectionMessages[Random(0, kRejectionMessages.size()-1)]、isRejected=trueでランダムな企業名とお祈りメッセージを設定

## このクラスのデストラクタ

デフォルトデストラクタを使用

## このクラスに含まれるデータメンバ

### public
- `String companyName` - 企業名（コンストラクタでランダム設定）
- `String message` - 不採用の場合お祈りメッセージ、採用の場合採用メッセージ（コンストラクタでランダム設定）
- `bool isRejected` - 採用結果（trueで不採用、falseで採用）

### private
- `static inline const Array<String> kCompanyNames` - 企業名のリスト（52社分）
- `static inline const Array<String> kRejectionMessages` - お祈りメッセージのリスト（約60メッセージ）
- `static inline const Array<String> kRecruitmentMessages` - 採用メッセージのリスト（約50メッセージ）

## このクラスに含まれる定数

- `static constexpr int32 kRejectedLabelFontSize = 24` - 「不採用」ラベルのフォントサイズ
- `static constexpr int32 kCompanyFontSize = 20` - 企業名のフォントサイズ
- `static constexpr int32 kMessageFontSize = 14` - お祈りメッセージのフォントサイズ
- `static constexpr ColorF kTextColor = UiConst::kUITextColor` - テキストの色
- `static constexpr ColorF kItemBgColor = UiConst::kInputAreaColor` - 項目の背景色(白系)
- `static constexpr ColorF kRejectedLabelColor{0.85, 0.05, 0.05}` - 「不採用」ラベルの色(赤、ラベル塗りつぶし用)
- `static constexpr ColorF kRecruitedLabelColor{0.05, 0.65, 0.15}` - 「採用」ラベルの色(緑、ラベル塗りつぶし用)
- `static constexpr ColorF kBorderColor{0.0, 0.0, 0.0}` - 枠線の色(黒)
- `static constexpr double kBorderThickness = 1.0` - 枠線の太さ
- `static constexpr double kCornerRadius = 0.0` - 角丸半径(角丸なし)

## このクラスに含まれるpublicメソッド

- `static RejectionInfo CreateRecruitmentInfo()` - 採用情報を生成する静的メソッド。info.companyName=kCompanyNames[Random]、info.message=kRecruitmentMessages[Random]、info.isRejected=falseで設定して返す
- `void Draw(const RectF& itemRect) const` - rejectedFont/companyFont/messageFont取得、itemRect.rounded(kCornerRadius).draw(kItemBgColor).drawFrame(kBorderThickness, kBorderColor)で項目背景描画。labelRect{itemRect.x+8.0, itemRect.y+6.0, 92.0, itemRect.h-12.0}でラベル矩形作成、labelColor=(isRejected?kRejectedLabelColor:kRecruitedLabelColor)で色選択、labelRect.rounded(2.0).draw(labelColor)で塗りつぶし。labelText=(isRejected?U"不採用":U"採用")でテキスト選択、rejectedFont(labelText).drawAt(kRejectedLabelFontSize, labelRect.center(), Palette::White)でラベル描画。companyPos={labelRect.x+labelRect.w+12.0, itemRect.y+12.0}で企業名位置計算、companyFont(companyName).draw(kCompanyFontSize, companyPos, kTextColor)で企業名描画。messagePos={companyPos.x+320.0, itemRect.y+12.0}でメッセージ位置計算、messageFont(message).draw(kMessageFontSize, messagePos, kTextColor)でメッセージ描画

## このクラスに含まれるprivateメソッド

なし

## このクラスで参照するアセットの情報

- フォント: `Asset/IBM_Plex_Sans_JP/IBMPlexSansJP-Regular.ttf` - IBM Plex Sans JPフォント

## このクラスが参照する仕様書の項目

- 求職活動フェーズ
- 不採用リストUI

## このクラスが使用されるフェーズ

- 求職活動フェーズ

## 特記事項・メモ

- 構造体からクラスに変更され、描画機能を持つようになった
- `isRejected`フィールドで不採用/採用を切り替え、ラベル色とテキストが変化する
- 企業名とメッセージはあらかじめ用意したリストからランダムに選択される(コンストラクタで設定)
- CreateRecruitmentInfo()静的メソッドで採用情報を生成可能
- 描画処理では「不採用」または「採用」ラベル（背景色付き）、企業名、メッセージを指定された矩形領域内に配置する
- ラベルは92.0幅、角丸2.0の矩形に白文字で中央寄せ描画
- 企業名はラベルの右側12.0ピクセル離れた位置、メッセージは企業名の右側320.0ピクセル離れた位置に配置
- 描画に関連する定数(フォントサイズ、色など)はこのクラスで管理する
- kCompanyNamesは52社、kRejectionMessagesは約60種類、kRecruitmentMessagesは約50種類のバリエーションを用意
