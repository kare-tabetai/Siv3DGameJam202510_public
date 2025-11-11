// DebugSystem.h
#pragma once
#include <Siv3D.hpp>

#include "Game/base_system/BlackOutUI.h"
#include "Game/base_system/PhaseManager.h"
#include "Game/base_system/GameCommonData.h"
#include "Game/utility/FontManager.h"

// デバッグ表示を行う静的ユーティリティクラス
// - F1 キーで表示/非表示を切り替え
// - FPS とマウス座標を左上に表示する
class DebugSystem {
  public:
  // 毎フレームの更新処理
  static void Update() {
    // F1 で表示トグル
    if (KeyF10.down()) {
      s_enabled = !s_enabled;
    }

    if (!s_enabled) {
      return;
    }

    // 表示用データを更新
    s_fps = Profiler::FPS();
    s_mousePos = Cursor::PosF();
  }

  // 描画処理（毎フレーム）
  static void Draw() {
    if (!s_enabled) {
      return;
    }

  // 情報表示用の半透明矩形（左下に表示）
  const int infoBgW = static_cast<int>(Scene::Width() * 0.4); // 画面幅の40%を目安
  const int infoBgH = 96; // 高さを広めにしてフェーズ名の切れを防ぐ
  const int margin = 8;
  const int infoX = margin;  // 左端から少し内側に寄せる
  const int infoY = static_cast<int>(Scene::Height()) - infoBgH;
  Rect(infoX, infoY, infoBgW, infoBgH).draw(ColorF(0.0, 0.0, 0.0, 0.45));

  // ボタン表示用の半透明矩形（情報表示の上に、下中央に表示）
  const int btnBgW = 320;
  const int btnBgH = 72;
  const int btnBgX = (static_cast<int>(Scene::Width()) - btnBgW) / 2;
  // 情報ボックスの上に重ならないように配置（info の上に表示）
  const int btnBgY = infoY - btnBgH - 10;
  Rect(btnBgX, btnBgY, btnBgW, btnBgH).draw(ColorF(0.0, 0.0, 0.0, 0.45));

  // テキストを描画（Font を毎フレーム生成しないよう FontManager から取得）
  const Font& font = FontManager::GetFont(U"ui_small");
  // 現在のフェーズ名を取得して表示に追加
  const String phaseName = PhaseManager::GetCurrentPhaseName();
  // 日付と精神力を追加表示
  const String currentDate = GameCommonData::GetCurrentDateString();
  const int32 currentMental = GameCommonData::GetMentalPower();
  const String info = U"FPS: " + Format(s_fps)
            + U"\nMouse: " + Format(s_mousePos.x) + U", " + Format(s_mousePos.y)
            + U"\nPhase: " + phaseName
            + U"\nDate: " + currentDate
            + U"\nMental: " + Format(currentMental);

  // 情報は左下（infoBg の内側）に描画
  font(info).draw(infoX + margin, infoY + margin, Palette::White);

    // デバッグ用ボタン（SimpleGUI を使用）：暗転解除 / 次フェーズへ
    // ボタン群を btnBg の内側で横方向中央揃えに配置
    const double buttonW = 140.0;
    const double gap = 20.0;  // ボタン間の隙間
    const double totalButtonsW = buttonW * 2.0 + gap;
    const double startBtnX = static_cast<double>(btnBgX) + (static_cast<double>(btnBgW) - totalButtonsW) / 2.0;
    const double btnY = static_cast<double>(btnBgY + (btnBgH - 30) / 2); // ボタン縦位置を中央寄せ
    const Vec2 btnPos(startBtnX, btnY);
    if (SimpleGUI::Button(U"暗転解除", btnPos, buttonW)) {
      BlackOutUI::HideImmediately();
      BlackOutUI::ClearMessage();
    }

    const Vec2 nextBtnPos(startBtnX + buttonW + gap, btnY);
    if (SimpleGUI::Button(U"次フェーズへ", nextBtnPos, buttonW)) {
      PhaseManager::ChangePhase(PhaseManager::GetNextPhaseType());
    }

    // --- 日付 / 精神力 操作用のボタン群 ---
    // 情報ボックスの内側下部に 3 つの小さめボタンを配置: 次の日へ, 精神力-, 精神力+
    const double smallBtnW = 88.0;
    const double smallGap2 = 8.0;
    const double totalSmallW = smallBtnW * 3.0 + smallGap2 * 2.0;
    const double startSmallX = static_cast<double>(infoX) + (static_cast<double>(infoBgW) - totalSmallW) - static_cast<double>(margin);
    const double smallBtnY = static_cast<double>(infoY + infoBgH - margin - 30);

    const Vec2 dateBtnPos(startSmallX, smallBtnY);
    if (SimpleGUI::Button(U"次の日へ", dateBtnPos, smallBtnW)) {
      GameCommonData::AdvanceDay();
    }

    const Vec2 decBtnPos(startSmallX + smallBtnW + smallGap2, smallBtnY);
    if (SimpleGUI::Button(U"精神力 -", decBtnPos, smallBtnW)) {
      GameCommonData::ChangeMentalPower(-1);
    }

    const Vec2 incBtnPos(startSmallX + (smallBtnW + smallGap2) * 2.0, smallBtnY);
    if (SimpleGUI::Button(U"精神力 +", incBtnPos, smallBtnW)) {
      GameCommonData::ChangeMentalPower(+1);
    }

    // --- フェーズ選択 UI (デバッグ) ---
    // ラジオボタンでフェーズ一覧を表示し、選択されたインデックスへ遷移する
    // ListBox の代替として SimpleGUI::RadioButtons を使用（縦並びの選択式）
    // フェーズ名一覧を動的に作成
    Array<String> phaseOptions;
    phaseOptions.reserve(phase_type_util::Count);
    for (int i = 0; i < phase_type_util::Count; ++i) {
      phaseOptions.push_back(phase_type_util::ToString(static_cast<PhaseType>(i)));
    }

    // 現在の選択インデックスが範囲外なら現在のフェーズに合わせる
    if (s_selectedPhaseIndex >= static_cast<size_t>(phase_type_util::Count)) {
      s_selectedPhaseIndex = static_cast<size_t>(PhaseManager::GetCurrentPhaseType());
    }

  // 画面右下（情報ボックスの上、右寄せ）に縦方向で配置
  const double listWidth = 240.0;
  const double jumpWidth = 92.0;
  const double smallGap = 12.0;
  const double totalListW = listWidth + smallGap + jumpWidth;
  const double listX = static_cast<double>(Scene::Width()) - margin - totalListW;
  // 下の要素が画面下で切れないように少し上側にずらす（40px 上へ移動）
  const double listY = static_cast<double>(infoY - 260); // info box の上に配置 (調整済み)
  const Vec2 listPos(listX, listY);

  // ラジオボタン（縦）を表示し、選択を受け取る（右下に表示）
  SimpleGUI::RadioButtons(s_selectedPhaseIndex, phaseOptions, listPos, listWidth);

  // フェーズへジャンプするボタン（ラジオリストの右側に配置）
  const Vec2 jumpBtnPos(listX + listWidth + smallGap, listY + 8.0);
    if (SimpleGUI::Button(U"ジャンプ", jumpBtnPos, 92.0)) {
      const size_t idx = s_selectedPhaseIndex;
      if (idx < static_cast<size_t>(phase_type_util::Count)) {
        PhaseManager::ChangePhase(static_cast<PhaseType>(static_cast<int>(idx)));
      }
    }
  }

  private:
  DebugSystem() = delete;

  // ヘッダオンリー対応のため inline static を使って変数を定義
  inline static bool s_enabled = true;         // 表示有無
  inline static int s_fps = 0;                 // 最終更新時の FPS
  inline static Vec2 s_mousePos = Vec2(0, 0);  // マウス座標
  inline static size_t s_selectedPhaseIndex = static_cast<size_t>(PhaseManager::GetCurrentPhaseType()); // デバッグ用選択インデックス
};
