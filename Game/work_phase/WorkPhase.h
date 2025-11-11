// WorkPhase.h
// アルバイトフェーズを管理するクラス

#pragma once
#include <Siv3D.hpp>

#include "Game/base_system/BlackOutUI.h"
#include "Game/base_system/CommonUI.h"
#include "Game/base_system/GameCommonData.h"
#include "Game/base_system/MessageWindowUI.h"
#include "Game/base_system/PhaseManager.h"
#include "Game/utility/FontManager.h"
#include "Game/utility/GameConst.h"
#include "Game/utility/MouseEffectManager.h"
#include "Game/utility/SoundManager.h"
#include "Game/utility/iPhase.h"
#include "MachineParts.h"

// アルバイトフェーズを管理するクラス
// 流れてくる部品のボルトを締める作業を行う
class WorkPhase : public iPhase {
  public:
  // フェーズ状態
  enum class State {
    ShowingInstruction,
    Running,
    EndMessage,  // 終了メッセージを表示して完了を待つ
    EndFadeWait  // 暗転フェードの終了を待つ
  };

  // 定数
  // ベルトのサイズ 3次元的には(x,z)
  static constexpr Vec2 kBeltSize{GameConst::kWorkAreaWidth, MachineParts::kBodySize.z + 80};
  static constexpr int32 kTargetCompletedCount = 5;  // 目標完了数
  // kPartsSpeed は MachineParts 側で使わなくなったため削除
  static constexpr Vec3 kPartsStartPos{-GameConst::kWorkAreaWidth / 2, 0.0, 0.0};  // 部品の開始位置（画面左外）
  static constexpr Vec3 kCameraPos{0.0, 150.0, -75.0};                             // カメラの位置
  static constexpr Vec3 kCameraTarget{0.0, 0.0, 0.0};                              // カメラの注視点
  static constexpr double kCameraFOV = 80_deg;                                     // カメラの視野角

  static constexpr ColorF kAmbientLightColor{0.35, 0.35, 0.35};  // 環境光の色
  static constexpr ColorF kSunColor{1.0, 0.98, 0.95};            // 平行光源の色
  static constexpr Vec3 kSunLightDirection{0.0, -1.0, 0.3};      // 平行光源の方向

  static constexpr double kInitialScrollSpeed = 200.2;  // ベルトのスクロール速度（m/秒）

  // ベルトコンベアのテクスチャパス
  static constexpr StringView kBeltTexturePath = U"Asset/gom_tex.png";

  // チェッカーボードテクスチャのパス
  static constexpr StringView kCheckerBoardTexturePath = U"Asset/checker_board_tex.png";

  // メタルボードテクスチャのパス
  static constexpr StringView kMetalBoardTexturePath = U"Asset/metal_board_tex.png";

  // 床(Plane)関連
  // カメラの見える範囲をカバーするためにワーク幅を基準に十分大きめに作る
  static constexpr Vec2 kFloorSize{900.0, 900.0};
  // テクスチャの1タイルあたりの大きさ (ワールド単位)。これで繰り返し回数を制御する
  static constexpr double kFloorTileSize = 200.0;
  // 床プレーンのオフセット
  static constexpr double kFloorYOffset = 0.5;

  // ベルトの淵用の薄いボックス設定
  static constexpr double kRimThickness = 10.0;  // ベルト淵の厚み（Z方向／ワールド単位）
  static constexpr double kRimHeight = 10.0;     // ベルト淵の高さ（Y方向の厚み）

  // コンストラクタ
  WorkPhase() {
    // カメラと光源を初期化
    SetupCamera();

    beltTexture_ = Texture{kBeltTexturePath};

    // ベルトを繰り返しテクスチャで表示するためのメッシュデータを作成して保存
    // MeshData::OneSidedPlane の第2引数で UV スケールや繰り返し回数を指定できる想定
    // const MeshData md = MeshData::OneSidedPlane(300, {1.0f, 1.0f}, Float2{uvOffset_, 0.0f});
    const MeshData md = CreateBeltMeshData();
    beltMesh_ = DynamicMesh(md);

    // 床用テクスチャとメッシュを作成（テクスチャは繰り返し）
    checkerTexture_ = Texture{kCheckerBoardTexturePath};
    // ベルト淵用の金属ボードテクスチャ
    metalTexture_ = Texture{kMetalBoardTexturePath};

    planeMesh_ = Mesh(MeshData::OneSidedPlane(kFloorSize, {10, 10}));

    // 初回プレイ時は操作説明を表示
    if (GameCommonData::IsFirstDay()) {
      ShowInstruction();
      state_ = State::ShowingInstruction;
    }

    // BGMを再生
    SoundManager::PlayBGM(U"bgm_factory");

    BackGroundManager::SetBackGround(U"");

    MouseEffectManager::SetActiveSound(false);

    // 最初の部品を生成
    SpawnNewParts();
  }

  // デストラクタ
  ~WorkPhase() = default;

  // 毎フレーム呼ばれる更新処理
  // 部品の更新、完了判定、フェーズ遷移を管理する
  void update() override {
    // ベルトのUVスクロールを更新
    uvOffset_ -= scrollSpeed_ * Scene::DeltaTime();

    // ベルトのメッシュを再作成
    const MeshData md = CreateBeltMeshData();
    beltMesh_.fill(md);

    // 状態ごとに更新処理を分岐
    switch (state_) {
      case State::ShowingInstruction:
        UpdateShowingInstruction(*this);
        break;
      case State::Running:
        UpdateRunningState();
        break;
      case State::EndMessage:
        UpdateEndMessage();
        break;
      case State::EndFadeWait:
        UpdateEndFadeWait();
        break;
      default:
        break;
    }
  }

  // 描画処理
  // 部品とカウントを描画する
  void draw() const override {
    // 背景色を設定
    Scene::SetBackground(ColorF{0.3, 0.3, 0.35});

    // 3D描画の準備
    {
      // 環境光を設定
      Graphics3D::SetGlobalAmbientColor(kAmbientLightColor);
      // Graphics3D::SetSunColor(kSunColor);
      // Graphics3D::SetSunDirection(kSunLightDirection);

      // カメラを設定
      Graphics3D::SetCameraTransform(camera_);

      // ベルトコンベアを描画（Mesh を使ってテクスチャを繰り返し表示）
      // 床をベルトよりわずかに下に描画（テクスチャは繰り返し）
      planeMesh_.draw(Vec3{0.0, static_cast<float>(kFloorYOffset), 0.0}, Quaternion::Identity(), checkerTexture_);

      // ベルトコンベアを描画（Mesh を使ってテクスチャを繰り返し表示）
      {
        const ScopedRenderStates3D sampler{SamplerState::MirrorAniso};
        beltMesh_.draw(beltTexture_);
      }

      // ベルトの上下の淵に沿って横長の薄い Box を描画
      // Box は中心位置で指定するので Y はオフセット + 高さ/2 を使う
      const float halfDepth = static_cast<float>(kBeltSize.y) / 2.0f;
      const float rimZTop = halfDepth - static_cast<float>(kRimThickness) / 2.0f;
      const float rimZBottom = -halfDepth + static_cast<float>(kRimThickness) / 2.0f;
      const float rimCenterY = static_cast<float>(kRimHeight / 2.0);
      // Box は中心位置とサイズで描画する。幅(X)=kBeltSize.x, 高さ(Y)=kRimHeight, 奥行き(Z)=kRimThickness
      const Vec3 rimSize{static_cast<float>(kBeltSize.x), static_cast<float>(kRimHeight), static_cast<float>(kRimThickness)};
      // 上下の Box を金属テクスチャで描画
      Box(Vec3{0.0f, rimCenterY, rimZTop}, rimSize).draw(metalTexture_);
      Box(Vec3{0.0f, rimCenterY, rimZBottom}, rimSize).draw(metalTexture_);

      // 部品を描画
      if (currentParts_.has_value()) {
        currentParts_->Draw();
      }
    }

    // 2D UIを描画
    {
      // 完了数を描画
      DrawCompletionCount();

      // 共通UIを描画
      CommonUI::Draw();

      // メッセージウィンドウを描画（操作説明用）
      if (state_ == State::ShowingInstruction) {
        MessageWindowUI::Draw();
      }
    }
  }

  private:
  // 操作説明中の更新処理を静的メンバ関数としてまとめる
  static void UpdateShowingInstruction(WorkPhase& self) {
    // メッセージが全て表示完了したら操作説明終了
    if (MessageWindowUI::IsCompleted()) {
      self.state_ = State::Running;
    }
  }
  void UpdateRunningState() {
    // 部品が存在する場合は更新
    if (currentParts_.has_value()) {
      currentParts_->Update(Scene::DeltaTime(), camera_, scrollSpeed_);
      // 部品の完了判定
      CheckPartsCompletion();
    }

    // 目標数に達したら終了メッセージ表示へ
    if (completedCount_ >= kTargetCompletedCount) {
      // BGMを停止
      SoundManager::StopBGM();

      // 終了メッセージを表示して EndMessage 状態へ移行
      Array<String> messages = {
        U"...",
        U"終業時間だ..."};
      MessageWindowUI::Show(messages);
      state_ = State::EndMessage;
    }
  }

  // 終了メッセージ表示中の更新処理
  void UpdateEndMessage() {
    // メッセージが全て表示完了したら暗転リクエストを出してフェード待ちへ
    if (MessageWindowUI::IsCompleted()) {
      BlackOutUI::FadeIn();
      state_ = State::EndFadeWait;
    }
  }

  // 暗転フェードの終了を待つ
  void UpdateEndFadeWait() {
    // フェード中でなければ（かつ暗転が見えていれば）次フェーズへ遷移
    if (!BlackOutUI::IsFading() && BlackOutUI::IsVisible()) {
      PhaseManager::ChangePhase(PhaseType::Sunset);
    }
  }

  MeshData CreateBeltMeshData() const {
    auto normalized = kBeltSize.normalized();
    auto uv_scale = Float2{kBeltSize.x / kBeltSize.y, 1.0f};
    const MeshData md = MeshData::Grid(kBeltSize, 1, 1, uv_scale, Float2{uvOffset_ / kBeltSize.y, 0.0f});
    return md;
  }

  // 新しい部品を生成して画面左から流す
  void SpawnNewParts() {
    currentParts_ = MachineParts{kPartsStartPos};
  }

  // 部品が画面外に出たか、ボルトを全て締めたかをチェック
  void CheckPartsCompletion() {
    if (!currentParts_.has_value()) {
      return;
    }

    // 画面外に出た場合
    // 部品の左端がkWorkAreaWidth/2よりも右に行ったら範囲外
    if (currentParts_->GetLeftEdge() > GameConst::kWorkAreaWidth / 2) {
      // 画面外に出た時点で念のため全ボルトが締められているかをチェックする
      if (currentParts_->IsCompleted()) {
        // 完了扱いとしてカウントを増やす
        completedCount_++;
      }

      // 現在の部品を破棄
      currentParts_.reset();

      // 次の部品を生成
      if (completedCount_ < kTargetCompletedCount) {
        SpawnNewParts();
      }
    }
  }

  // 初回時の操作説明をMessageWindowUIに表示
  void ShowInstruction() {
    Array<String> messages = {
      U"部品が左から流れてきます",
      U"六角ボルトをクリックして締めてください",
      U"全てのボルトを締めれば作業完了です"};
    MessageWindowUI::Show(messages);
  }

  // 完了数を画面上部に描画
  void DrawCompletionCount() const {
    const Font& font = FontManager::GetFont(U"ui_medium");
    const String countStr = U"完了: {}/{}"_fmt(completedCount_, kTargetCompletedCount);
    const Vec2 pos{GameConst::kPlayAreaSize.x / 2, GameConst::kPlayAreaRect.y + 20};
    font(countStr).draw(Arg::center = pos, ColorF{1.0, 1.0, 1.0});
  }

  // カメラと光源の初期設定を行う。コンストラクタから呼び出される
  void SetupCamera() {
    // カメラを設定
    camera_ = BasicCamera3D{Scene::Size(), kCameraFOV, kCameraPos, kCameraTarget};
  }

  // データメンバ
  Optional<MachineParts> currentParts_;  // 現在流れている部品（存在しない場合はnone）
  int32 completedCount_ = 0;             // 完了した部品数

  State state_ = State::Running;  // 現在の状態

  BasicCamera3D camera_;                      // 3D描画用のカメラ
  Mesh planeMesh_;                            // 床用メッシュ
  Texture checkerTexture_;                    // チェッカーボードのテクスチャ
  DynamicMesh beltMesh_;                      // ベルト用メッシュ（繰り返し表示用）
  Texture beltTexture_;                       // ベルトコンベアのテクスチャ
  Texture metalTexture_;                      // ベルト淵用の金属テクスチャ
  double scrollSpeed_ = kInitialScrollSpeed;  // ベルトのスクロール速度
  double uvOffset_ = 0.0;                     // ベルトのUVオフセット（スクロール位置）
};
