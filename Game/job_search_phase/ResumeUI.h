// ResumeUI.h
#pragma once
#include <Siv3D.hpp>

#include "Game/utility/ConfirmDialog.h"
#include "Game/utility/FontManager.h"
#include "Game/utility/GameConst.h"
#include "Game/utility/SoundManager.h"
#include "Game/utility/UiConst.h"

// 履歴書の表示と入力を管理するUIクラス
// 基本情報・職歴・病歴・資格は自動入力され、自己PR/志望動機のみプレイヤーが入力できる
class ResumeUI {
  public:
  enum class State {
    WaitLogIn,
    AutoComplete,
    WaitInput,
  };

  // レイアウト関連定数
  static constexpr double kResumeWidth = GameConst::kPlayAreaSize.x * 5 / 6;   // 履歴書の幅
  static constexpr double kResumeHeight = GameConst::kPlayAreaSize.y * 7 / 8;  // 履歴書の高さ
  static constexpr Vec2 kResumeSize = {kResumeWidth, kResumeHeight};           // 履歴書のサイズ

  // 履歴書の中心位置
  static constexpr Vec2 kResumeCenterPos = GameConst::kPlayAreaCenter;
  static constexpr Vec2 kResumeLeftTopPos = kResumeCenterPos - kResumeSize / 2;  // 履歴書の左上位置
  static constexpr RectF kResumeRect{kResumeLeftTopPos, kResumeSize};            // 履歴書の描画領域（デフォルト）

  static constexpr double kCornerRadius = 0.0;   // 角丸半径（角丸なし）
  static constexpr double kWindowMargin = 20.0;  // ウィンドウの余白
  static constexpr double kAreaMargin = 10.0;    // エリアの余白

  // 基本情報エリア関連定数
  static constexpr Vec2 kBasicInfoAreaPos = kResumeLeftTopPos + Vec2::One() * kWindowMargin;       // 基本情報エリアの位置
  static constexpr Vec2 kBasicInfoAreaSize = (kResumeSize - 3 * Vec2::One() * kWindowMargin) / 2;  // 基本情報エリアのサイズ
  static constexpr ColorF kBasicInfoBorderColor{0.0, 0.0, 0.0};                                    // 基本情報エリアの枠線色

  // 職歴・病歴・資格エリア関連定数
  static constexpr Vec2 kWorkHistoryAreaPos = kBasicInfoAreaPos + Vec2{0, kBasicInfoAreaSize.y + kWindowMargin};  // 職歴・病歴・資格エリアの位置
  static constexpr ColorF kWorkHistoryBorderColor{0.0, 0.0, 0.0};                                                 // 職歴・病歴・資格エリアの枠線色

  // 自己PRエリア関連定数
  static constexpr Vec2 kSelfPRAreaPos = kBasicInfoAreaPos + Vec2{kWindowMargin + kBasicInfoAreaSize.x, 0};  // 自己PRエリアの位置
  static constexpr Vec2 kSelfPRAreaSize{kBasicInfoAreaSize.x, kResumeSize.y - kWindowMargin * 2};            // 自己PRエリアのサイズ

  static constexpr Vec2 kSubmitButtonSize{100, 40};           // 決定ボタンのサイズ
  static constexpr ColorF kSelfPRBorderColor{0.0, 0.0, 0.0};  // 自己PRエリアの枠線色

  // フォント関連定数
  static constexpr int32 kHeaderFontSize = 32;  // 見出しのフォントサイズ
  static constexpr int32 kBodyFontSize = 20;    // 本文のフォントサイズ
  static constexpr int32 kSmallFontSize = 16;   // 小さい文字のフォントサイズ

  // 色関連定数
  static constexpr ColorF kTextColor = UiConst::kUITextColor;                        // テキストの色（黒）
  static constexpr ColorF kResumeBackgroundColor = UiConst::kWindowColor;            // 履歴書ウィンドウの背景色
  static constexpr ColorF kWindowBorderColor = UiConst::kWindowBorderColor;          // ウィンドウ枠の色
  static constexpr double kWindowBorderThickness = UiConst::kWindowBorderThickness;  // ウィンドウ枠の太さ
  static constexpr ColorF kInputAreaColor = UiConst::kInputAreaColor;                // 入力エリアの背景色（白）
  // 無効化時のオーバーレイ色（黒：半透明）
  static constexpr ColorF kDisabledOverlayColor{0.0, 0.0, 0.0, 0.5};

  // その他定数
  static constexpr int32 kMaxSelfPRLength = 100;        // 自己PR/志望動機の最大文字数
  static constexpr double kAutoCompleteInterval = 0.4;  // オートコンプリートの入力間隔（秒）

  // 固定データ
  static constexpr int kDefaultRejectionCount = 81;                               // ゲームが始まるまでに不採用になっていた回数
  static inline const String kNationalID = U"KU1623142";                          // 国民番号
  static inline const String kGender = U"男";                                     // 生物学的性別
  static inline const String kAge = U"26歳";                                      // 年齢
  static inline const String kAddress = U"千葉県船橋市日の出区平井3123-456-789";  // 住所
  static inline const String kJobRank = U"E";                                     // 雇用階級
  static inline const String kWageRank = U"D";                                    // 賃金階級
  static inline const String kEducationalRank = U"A";                             // 学歴階級
  // 職歴
  static inline const Array<String> kWorkHistory =
    {
      U"2029年3月 横浜工業大学 工学研究科 修士課程終了\n",
      U"2029年4月〜2030年1月 サイバーフューチャー株式会社 正社員\n",
      U"2030年10月〜2031年3月 芙蓉テクノホールディングス 正社員"};

  // 資格
  static inline const Array<String> kQualifications =
    {
      U"普通自動車免許(AI限)\n",
      U"高等学校教諭一種免許状\n",
      U"応用情報技術者"};

  // 履歴書UIを表示開始する
  void Show() {
    // 初期化: 確定フラグ・自己PRはクリア
    isConfirmed_ = false;
    selfPREditState_ = TextAreaEditState();

    // 状態を初期化してタイマーをリセット
    autoCompleteIndex_ = 0;
    stopwatch_.reset();

    confirmDialog_ = ConfirmDialog();
  }

  void Update() {
    // 確認ダイアログの更新
    confirmDialog_.Update();

    // 確認ダイアログで「はい」が押されたら確定
    if (confirmDialog_.IsConfirmed()) {
      isConfirmed_ = true;
    }

    switch (state_) {
      case State::WaitLogIn:

        break;

      case State::AutoComplete:
        UpdateAutoComplete();
        break;

      case State::WaitInput:
        break;

      default:
        break;
    }
  }

  // 毎フレーム呼ばれる描画処理。履歴書UIを描画する
  void Draw() const {
    DrawResumeBackground();
    DrawBasicInfo();
    DrawWorkHistory();
    DrawSelfPRArea();

    // 無効化されているように見せるためのオーバーレイを表示（enabled_ が false のとき）
    if (grayout_) {
      // 履歴書ウィンドウ全体を覆う半透明の矩形（色は定数化）
      kResumeRect.rounded(kCornerRadius).draw(kDisabledOverlayColor);
    }

    // 確認ダイアログの描画（ダイアログはオーバーレイの上に表示される）
    confirmDialog_.Draw();
  }

  // 入力が確定されたかどうかを返す
  [[nodiscard]] bool IsConfirmed() const noexcept { return isConfirmed_; }

  // 入力された自己PR/志望動機を取得する
  [[nodiscard]] String GetSelfPR() const { return selfPREditState_.text; }

  // 公開API: テキストエリアと送信ボタンの有効/無効を単一フラグで管理する
  void SetInputEnabled(bool enabled) noexcept { enabled_ = enabled; }
  [[nodiscard]] bool IsEnabled() const noexcept { return enabled_; }
  [[nodiscard]] State GetState() const noexcept { return state_; }
  void SetGrayOut(bool grayout) noexcept { grayout_ = grayout; }

  void StartAutoComplete() {
    assert(state_ == State::WaitLogIn);
    state_ = State::AutoComplete;
    autoCompleteIndex_ = 0;
    state_ = State::AutoComplete;
    // オートコンプリート演出のタイマー開始は状態遷移時に行う
    stopwatch_.restart();
  }

  private:
  // 履歴書の背景を描画する
  void DrawResumeBackground() const {
    // 定義済みの履歴書領域を描画する
    // （resumeRect をクラス定数 kResumeRect として昇格させた）
    kResumeRect.rounded(kCornerRadius)
      .draw(kResumeBackgroundColor)
      .drawFrame(2, ColorF{0.0, 0.0, 0.0});
  }

  // 基本情報欄を描画する（左上のボックス）
  void DrawBasicInfo() const {
    const RectF basicInfoRect{kBasicInfoAreaPos, kBasicInfoAreaSize};
    basicInfoRect.rounded(kCornerRadius).drawFrame(2, kBasicInfoBorderColor);

    const Font& headerFont = FontManager::GetFont(U"resume_medium");
    const Font& bodyFont = FontManager::GetFont(U"resume_small");

    auto header_rect = headerFont(U"基本情報").draw(kHeaderFontSize, kBasicInfoAreaPos + Vec2{kAreaMargin, kAreaMargin}, kTextColor);

    bodyFont(U"国民番号: " +
             nationalID_ +
             U"\n"
             U"生物学的性別: " +
             gender_ +
             U"\n"
             U"年齢: " +
             age_ +
             U"\n"
             U"住所: " +
             address_ +
             U"\n"
             U"職制階級: " +
             jobRank_ +
             U"\n"
             U"賃金階級: " +
             wageRank_ +
             U"\n"
             U"学歴階級: " +
             educationalRank_ +
             U"\n"
             U"不採用回数: " +
             rejectionCount_)
      .draw(kSmallFontSize, header_rect.bl(), kTextColor);
  }

  // 職歴・病歴・資格欄を描画する（左下のボックス）
  void DrawWorkHistory() const {
    const RectF workHistoryRect{kWorkHistoryAreaPos, kBasicInfoAreaSize};
    workHistoryRect.rounded(kCornerRadius)
      .drawFrame(2, kWorkHistoryBorderColor);

    const Font& headerFont = FontManager::GetFont(U"resume_medium");
    const Font& bodyFont = FontManager::GetFont(U"resume_small");

    auto header_rect = headerFont(U"職歴・資格").draw(kHeaderFontSize, kWorkHistoryAreaPos + Vec2{kAreaMargin, kAreaMargin}, kTextColor);

    String workHistory = U"";
    for (const auto& line : workHistory_) {
      workHistory += line;
    }
    auto job_body_rect = bodyFont(workHistory).draw(kSmallFontSize, header_rect.bl(), kTextColor);

    String qualifications = U"";
    for (const auto& line : qualifications_) {
      qualifications += line;
    }
    bodyFont(U"\n" + qualifications).draw(kSmallFontSize, job_body_rect.bl(), kTextColor);
  }

  // 自己PR/志望動機欄を描画する（右側の大きなボックス）
  void DrawSelfPRArea() const {
    const RectF selfPRRect{kSelfPRAreaPos, kSelfPRAreaSize};
    selfPRRect.rounded(kCornerRadius).drawFrame(2, kSelfPRBorderColor);

    const Font& headerFont = FontManager::GetFont(U"resume_medium");
    auto header_rect = headerFont(U"自己PR(" + Format(kMaxSelfPRLength) + U"文字以内)").draw(kHeaderFontSize, kSelfPRAreaPos + Vec2{10, 10}, kTextColor);

    const Vec2 kSelfPRInputAreaPos = header_rect.bl();
    const double kPrInputAreaWidth = kSelfPRAreaSize.x - kAreaMargin * 2;
    const double kPrInputAreaHeight = kSelfPRAreaSize.y - kSubmitButtonSize.y - kAreaMargin * 4 - header_rect.h;

    const Vec2 kSelfPRInputAreaSize = {kPrInputAreaWidth, kPrInputAreaHeight};  // 自己PR入力エリアのサイズ
    // controlsEnabled(): 外部フラグ(enabled_) と確認ダイアログの表示状態の両方を考慮して有効/無効を決定
    SimpleGUI::TextArea(selfPREditState_, kSelfPRInputAreaPos, kSelfPRInputAreaSize, kMaxSelfPRLength, ControlsEnabled());

    // 文字数カウンターを描画（テキストエリアの左下）
    const size_t currentLength = selfPREditState_.text.size();
    const String counterText = U"({}/{}文字)"_fmt(currentLength, kMaxSelfPRLength);
    const Font& counterFont = FontManager::GetFont(U"resume_small");
    const Vec2 counterPos = kSelfPRInputAreaPos + Vec2{kAreaMargin, kSelfPRInputAreaSize.y + 5};
    counterFont(counterText).draw(kSmallFontSize, counterPos, kTextColor);

    // 送信ボタンを描画
    if (ControlsEnabled()) {
      const double kbuttonPosX = kSelfPRAreaPos.x + kSelfPRAreaSize.x / 2;
      const double kbuttonPosY = kSelfPRAreaPos.y + kSelfPRAreaSize.y - kSubmitButtonSize.y - kAreaMargin;
      const Vec2 buttonCenterPos = {kbuttonPosX, kbuttonPosY + kSubmitButtonSize.y / 2};
      const RectF buttonRect{buttonCenterPos - kSubmitButtonSize / 2, kSubmitButtonSize};

      const bool isHovered = buttonRect.mouseOver();
      const ColorF buttonColor =
        isHovered ? ColorF{0.3, 0.5, 0.7} : ColorF{0.2, 0.4, 0.6};

      buttonRect.rounded(kCornerRadius)
        .draw(buttonColor)
        .drawFrame(2, ColorF{0.0, 0.0, 0.0});

      const Font& font = FontManager::GetFont(U"ui_medium");
      font(U"送信").drawAt(20, buttonRect.center(), Palette::White);

      // 決定ボタンのクリック判定と処理
      if (buttonRect.leftClicked()) {
        confirmDialog_.Show(U"この内容で提出しますか？", kResumeCenterPos);
      }
    }
  }

  // キーボード打鍵音をランダムに再生する
  void PlayTypingSound() {
    const Array<String> typingSounds = {
      U"se_keyboard1",
      U"se_keyboard2",
      U"se_keyboard3"};
    const String selectedSound =
      typingSounds[Random<size_t>(0, typingSounds.size() - 1)];
    SoundManager::PlaySE(selectedSound, 0.3);  // 音量を小さめに
  }

  // 日付をフォーマットする（例: 20310622 -> "2031/06/22"）
  [[nodiscard]] String FormatDate(int32 date) const {
    const int32 year = date / 10000;
    const int32 month = (date / 100) % 100;
    const int32 day = date % 100;
    return U"{:04d}/{:02d}/{:02d}"_fmt(year, month, day);
  }

  void UpdateAutoComplete() {
    // Auto-complete演出: 一定間隔ごとに項目単位でフィールドを補完していく
    // 順序: 基本情報(国民番号, 生物学的性別, 住所) -> 職歴 -> 資格
    // 補完ごとに autoCompleteIndex_ をインクリメントし、全て完了したら WaitInput に遷移する

    // 指定間隔経過で次の項目を補完
    if (stopwatch_.sF() >= kAutoCompleteInterval) {
      // オートコンプリートで扱う項目数（基本情報、職歴/資格は配列要素数）
      const int kBasicCount = 8;
      const int kWorkCount = static_cast<int>(kWorkHistory.size());
      const int kQualCount = static_cast<int>(kQualifications.size());

      const int totalCount = kBasicCount + kWorkCount + kQualCount;

      if (autoCompleteIndex_ < totalCount) {
        // 基本情報
        if (autoCompleteIndex_ < kBasicCount) {
          switch (autoCompleteIndex_) {
            case 0:
              nationalID_ = kNationalID;
              break;
            case 1:
              gender_ = kGender;
              break;
            case 2:
              age_ = kAge;
              break;
            case 3:
              address_ = kAddress;
              break;
            case 4:
              jobRank_ = kJobRank;
              break;
            case 5:
              wageRank_ = kWageRank;
              break;
            case 6:
              educationalRank_ = kEducationalRank;
              break;
            case 7:
              rejectionCount_ = CountRejectionCount();
              break;
            default:
              break;
          }
        } else if (autoCompleteIndex_ < kBasicCount + kWorkCount) {
          // 職歴: kWorkHistory の該当要素を push_back して表示させる
          const int idx = autoCompleteIndex_ - kBasicCount;
          if (idx >= 0 && idx < kWorkCount) {
            workHistory_.push_back(kWorkHistory[idx]);
          }
        } else {
          // 資格: kQualifications の該当要素を push_back
          const int idx = autoCompleteIndex_ - kBasicCount - kWorkCount;
          if (idx >= 0 && idx < kQualCount) {
            qualifications_.push_back(kQualifications[idx]);
          }
        }

        // 次の項目へ進む準備
        ++autoCompleteIndex_;
        stopwatch_.restart();
      } else {
        // 全項目補完完了: 自己PR入力待ちへ遷移
        state_ = State::WaitInput;
        stopwatch_.reset();
      }
    }
  }

  static const String CountRejectionCount() {
    int count = kDefaultRejectionCount + GameCommonData::GetCurrentDay();
    return Format(count, U"回");
  }

  // データメンバ
  // 定数で初期化したメンバ変数（DrawBasicInfo 等で直接定数を参照しないため）
  String nationalID_ = U"";            // 国民番号（定数から初期化）
  String gender_ = U"";                // 生物学的性別（定数から初期化）
  String age_ = U"";                   // 年齢（定数から初期化）
  String address_ = U"";               // 住所（定数から初期化）
  String wageRank_ = U"";              // 賃金階級（定数から初期化）
  String jobRank_ = U"";               // 雇用階級（定数から初期化）
  String educationalRank_ = U"";       // 学歴階級（定数から初期化）
  String rejectionCount_ = U"";        // 不採用回数
  Array<String> workHistory_ = {};     // 職歴（定数で初期化）
  Array<String> qualifications_ = {};  // 資格（定数で初期化）

  mutable TextAreaEditState selfPREditState_;  // 自己PR/志望動機の入力状態を管理するSiv3Dの編集状態オブジェクト
  mutable bool isConfirmed_ = false;           // 入力内容が確定されたかどうか
  mutable ConfirmDialog confirmDialog_;        // 確認ダイアログ
  // 外部からUI入力の有効/無効を切り替えるための単一フラグ
  mutable bool enabled_ = true;
  mutable bool grayout_ = true;

  State state_ = State::WaitLogIn;
  Stopwatch stopwatch_;
  int autoCompleteIndex_ = 0;  // 自動入力中の項目インデックス

  // enabled_ と確認ダイアログの表示状態を組み合わせて実際に操作可能かを返す
  [[nodiscard]] bool ControlsEnabled() const noexcept { return enabled_ && !confirmDialog_.IsVisible(); }
};
