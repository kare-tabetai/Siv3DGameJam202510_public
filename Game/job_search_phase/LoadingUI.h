// LoadingUI.h
#pragma once
#include <Siv3D.hpp>
#include "ServerLoadingUI.h"
#include "Game/utility/FontManager.h"
#include "Game/utility/UiConst.h"
#include "Game/utility/RotatingIcon.h"

// TODO: 画面サイズを基準としたサイズ、位置の指定を行うように定数を調整する

// LLMによる履歴書評価の待ち時間中に表示されるローディング画面UIクラス
// 世界観を説明する広告文章を8秒ごとに切り替えて表示し、プレイヤーの待ち時間を軽減しつつ世界観の没入感を高める
class LoadingUI {

  public:
  // 定数
  static constexpr double kAdChangeInterval = 8.0;  // 広告切り替え間隔(秒)
  // アイコンのスケール変調（サイン波）設定: 1.0 ± 0.2 -> 範囲 [0.8, 1.2]
  static constexpr Vec2 kDialogSize{1100, 425};  // ダイアログのサイズ(イメージ図に基づく)
  static constexpr Vec2 kDialogPos{90, 205};  // ダイアログの表示位置(画面左上からの絶対位置)
  static constexpr double kDialogRadius = UiConst::kUiWindowRadius;  // ダイアログの角丸半径（共通設定を利用）
  static constexpr int32 kAdFontSize = 24;  // 広告文章のフォントサイズ
  static constexpr int32 kLoadingFontSize = 32;  // 「評価中」のフォントサイズ
  static constexpr double kIconSize = 60.0;  // ローディングアイコンのサイズ(六角形の外接円の直径)
  // ロゴ表示の定数
  static constexpr double kBottomLogoSize = 80.0;  // 下部ロゴの表示サイズ（ピクセル）
  static constexpr Vec2 kBottomLogoOffset{40, 325};  // ダイアログ左上からの下部ロゴオフセット
  // 色は共通設定に合わせる
  static constexpr ColorF kTextColor = UiConst::kUITextColor;  // テキストの色
  static constexpr ColorF kBackgroundColor = UiConst::kWindowColor;  // 背景色（ダイアログの背景に合わせる）
  static constexpr Vec2 kLoadingTextPos{640, 318};  // 「評価中」テキストの表示位置(画面中央やや上)
  static constexpr Vec2 kIconPos{640, 380};  // ローディングアイコンの表示位置(「評価中」の下)
  static constexpr Vec2 kAdTextPos{240, 525};  // 広告文章の表示位置（ダイアログ内左下）
  static constexpr double kAdTextWidth = 900.0;  // 広告文章の表示幅（折り返し用、ダイアログ幅より少し小さく）

  // アセットパス
  static inline const String kLoadingLogoPath = String(UiConst::kMonoEyeIconPath);  // ロード中ロゴ画像のパス（共通定義を使用）

  // 表示する広告文章のリスト
  static inline const Array<String> kAdvertisements = {
      U"モノアイは最新のAI技術を駆使し、あなたに最適な職場を提案します。\n今すぐモノアイで理想のキャリアを見つけましょう!",
      U"モノアイを利用した求職活動は１日につき１度までです。",
      U"モノアイを介さない企業の採用活動(闇採用)は、違法行為です!!",
      U"モノアイ最新版にて、国民番号データベースとの連携機能を追加しました。\nデータベースの情報をもとに面倒だった入力項目を補完できます。",
      U"プロンプトインジェクションを用いたモノアイの利用は、犯罪です。\n法により5年以下の懲役、または1000万円以下の罰金が科せられることがあります。"
  };

  // ローディングUIを表示開始する。タイマーをリセットし、広告インデックスを初期化する
  void Show() {
    isVisible_ = true;
    // 広告の初期インデックスは表示時にランダムで決定する
    currentAdIndex_ = Random(0, static_cast<int32>(kAdvertisements.size()) - 1);
    adChangeTimer_ = Timer{SecondsF{kAdChangeInterval}, StartImmediately::Yes };

    // 回転アイコンを初期化して表示（ロゴは LoadingUI が保持して描画する）
    rotatingIcon_.Initialize( kIconSize, kIconPos);
    rotatingIcon_.Show();

    // ロゴは LoadingUI 側で管理して描画する
    logoTexture_ = Texture{kLoadingLogoPath};
  }

  // ローディングUIを非表示にする
  void Hide() {
    isVisible_ = false;
  }

  // 毎フレーム呼ばれる更新処理。広告の切り替えとアイコンの回転を行う
  void Update() {
    if (!isVisible_) {
      return;
    }

    UpdateAdvertisement();
    rotatingIcon_.Update();
  }

  // 毎フレーム呼ばれる描画処理。ダイアログ、広告文章、ローディングアイコンを描画する
  void Draw() const {
    if (!isVisible_) {
      return;
    }

    DrawDialog();
    rotatingIcon_.Draw();
    DrawLoadingIcon();
    DrawAdvertisement();
  }

  // UIが表示中かどうかを返す
  [[nodiscard]] bool IsVisible() const noexcept {
    return isVisible_;
  }

  private:
  // 広告の切り替え処理
  void UpdateAdvertisement() {
    if (adChangeTimer_.reachedZero()) {
      currentAdIndex_ = (currentAdIndex_ + 1) % kAdvertisements.size();
      adChangeTimer_.restart();
    }
  }

  // ダイアログ背景を描画する
  void DrawDialog() const {
    const RectF dialogRect{kDialogPos, kDialogSize};
    dialogRect.rounded(kDialogRadius).draw(kBackgroundColor)
      .drawFrame(UiConst::kWindowBorderThickness, UiConst::kWindowBorderColor);

    // 「評価中」テキストを描画
    const Font& font = FontManager::GetFont(U"ui_large");
    // FontAsset のサイズは事前定義されているため、drawAt には位置と色を渡す
    font(U"評価中").drawAt(kLoadingTextPos, kTextColor);
  }

  // 現在の広告文章を描画する
  void DrawAdvertisement() const {
    if (currentAdIndex_ >= static_cast<int32>(kAdvertisements.size())) {
      return;
    }

    const Font& font = FontManager::GetFont(U"ui_medium");
    const String& ad = kAdvertisements[currentAdIndex_];
    // プロジェクト内の他UIと同様にフォントサイズを指定して描画する（折り返しはフォント側で処理される）
    font(ad).draw(kAdFontSize, kAdTextPos, kTextColor);
  }

  // 回転するローディングアイコンを描画する
  void DrawLoadingIcon() const {
    // ロゴをダイアログ左下に表示（ロゴ描画は LoadingUI が担当）
    if (logoTexture_) {
      logoTexture_.resized(kBottomLogoSize).draw(kDialogPos + kBottomLogoOffset);
    }
  }

  // データメンバ
  int32 currentAdIndex_ = 0;           // 現在表示中の広告のインデックス
  Timer adChangeTimer_;                // 広告切り替え用のタイマー
  bool isVisible_ = false;             // UIが表示中かどうか
  RotatingIcon rotatingIcon_;
  Texture logoTexture_;                // 下部ロゴは LoadingUI が保持して描画する
  ServerLoadingUI serverLoadingUI_;
};
