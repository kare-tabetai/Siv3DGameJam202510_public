// HexBolt.h
// 六角ボルトの描画と管理を行うクラス

#pragma once
#include <Siv3D.hpp>
#include "Game/utility/SoundManager.h"
#include "Game/utility/TweenUtil.h"

// 六角ボルトの描画と管理を行うクラス
// クリック判定を検知し、締められたかどうかの状態を保持する
class HexBolt {
public:
	// コンストラクタ
	explicit HexBolt(const Vec3& position)
		: position_(position)
		, isTightened_(false) {
		// 六角柱メッシュを生成（Cylinderを6分割）
		boltMesh_ = Mesh{MeshData::Cylinder({0, 0, 0}, kBoltRadius, kBoltHeight, 6)};

		// マテリアルを初期化（デフォルト色）
		material_ = PhongMaterial{kDefaultColor};
	}

	// デストラクタ
	~HexBolt() = default;

	// 毎フレーム呼ばれる更新処理
	// parentDelta: 親（部品）がこのフレームに移動した量。ボルト位置を親と一緒に移動させる
	// クリック判定を行う。3D→2D座標変換のためにカメラ情報を引数で受け取る
	void Update(const BasicCamera3D& camera, const Vec3& parentDelta) {
		// まず親の移動分を反映する（締められた後でも位置は移動させる）
		position_ += parentDelta;

		// アニメーション中なら経過時間を進める
		if (isAnimating_) {
			animationElapsed_ += Scene::DeltaTime();
			const double progress = Min(animationElapsed_ / kAnimationDuration, 1.0);

			// イージングを適用して回転（始めは速く、終わりにかけてなだらかに）
			const double eased = TweenUtil::EaseOutCubic(progress);
			rotation_ = Quaternion::RotateY(-eased * kTargetRotation);

			if (progress >= 1.0) {
				// アニメーション完了: 締めた状態にする
				isAnimating_ = false;
				isTightened_ = true;
				material_ = PhongMaterial{kTightenedColor};
			}

			// アニメーション中はクリック判定を受け付けない
			return;
		}

		// 既に締められている場合はそれ以上の処理は不要
		if (isTightened_) {
			return;
		}

		// マウスがボルト上にあり、クリックされたらアニメーション開始
		if (IsMouseOver(camera) && MouseL.down()) {
			OnClick();
		}
	}

	// ボルトを描画する
	// 締められているかどうかで色を変える
	void Draw() const {
		// 回転を反映してメッシュを描画（ボルトは中心が原点なので、回転→平行移動の順）
		boltMesh_.draw(position_, rotation_, material_);
	}

	// 締められたかどうかを返す
	[[nodiscard]] bool IsTightened() const noexcept {
		return isTightened_;
	}

	// ボルトの3D空間上の位置を返す
	[[nodiscard]] Vec3 GetPosition() const noexcept {
		return position_;
	}

	static constexpr double kBoltRadius = 10.0;        // ボルトの半径
	static constexpr double kBoltHeight = 5.0;        // ボルトの高さ
	static constexpr double kClickRadius = 20.0;       // クリック判定の半径（ボルトより少し大きめ）
	// アニメーション設定
	static constexpr double kAnimationDuration = 0.4; // クリックから締め終わりまでの時間（秒）
	static constexpr double kTargetRotation = Math::TwoPi * 2; // 1回転 (ラジアン)
	static inline const ColorF kDefaultColor{0.4, 0.4, 0.45};     // デフォルトの色（灰色）
	static inline const ColorF kTightenedColor{0.6, 0.6, 0.65};   // 締めた後の色（明るい灰色）

private:
	// クリックされた時の処理
	// 締める音を再生し、状態を変更する
	void OnClick() {
		// 既に締められている、またはアニメーション中は何もしない
		if (isTightened_ || isAnimating_) {
			return;
		}

		// アニメーション開始
		isAnimating_ = true;
		animationElapsed_ = 0.0;
		rotation_ = Quaternion::Identity();

		// レンチ音を再生
		SoundManager::PlaySE(U"se_wrench");
	}

	// マウスがボルト上にあるかどうかを判定する
	// 3D→2D座標変換のためにカメラ情報を引数で受け取る
	[[nodiscard]] bool IsMouseOver(const BasicCamera3D& camera) const {
		// Cursorモジュールを使ってレイキャストでクリック判定を行う
		// より簡易的に、ボルトの位置を中心とした球体での判定を行う
		const Ray ray = camera.screenToRay(Cursor::Pos());
		const Sphere clickSphere{position_, kClickRadius};

		// レイと球体の交差判定
		return ray.intersects(clickSphere).has_value();
	}

	// データメンバ
	Vec3 position_;              // ボルトの3D空間上の位置
  Quaternion rotation_ = Quaternion::Identity(); // 現在の回転（アニメーション用）
	bool isTightened_ = false;   // 締められたかどうか
	// アニメーション関連
	bool isAnimating_ = false;    // 締めるアニメーション中か
	double animationElapsed_ = 0.0; // アニメーション経過時間 (秒)
	Mesh boltMesh_;              // ボルトのメッシュ（六角柱）
	PhongMaterial material_;     // マテリアル
};
