// TweenUtil.h
// ツウィーン（イージング）関数を提供する静的ユーティリティクラス

#pragma once
#include <Siv3D.hpp>

// ツウィーン（イージング）関数を提供する静的ユーティリティクラス
// UIアニメーションの補間計算に使用する
class TweenUtil {
public:
	// コンストラクタは削除（静的クラスのため、インスタンス化不可）
	TweenUtil() = delete;

	// 線形補間（そのまま返す）。t ∈ [0, 1]
	[[nodiscard]] static double Linear(double t) noexcept {
		return t;
	}

	// イーズイン（加速）。二次関数カーブ（t²）
	[[nodiscard]] static double EaseIn(double t) noexcept {
		return t * t;
	}

	// イーズアウト（減速）。二次関数カーブ（1 - (1-t)²）
	[[nodiscard]] static double EaseOut(double t) noexcept {
		return 1.0 - (1.0 - t) * (1.0 - t);
	}

	// イーズインアウト（加速→減速）。三次関数カーブ
	[[nodiscard]] static double EaseInOut(double t) noexcept {
		if (t < 0.5) {
			return 2.0 * t * t;
		} else {
			return 1.0 - 2.0 * (1.0 - t) * (1.0 - t);
		}
	}

	// イーズイン（三次）。より強い加速（t³）
	[[nodiscard]] static double EaseInCubic(double t) noexcept {
		return t * t * t;
	}

	// イーズアウト（三次）。より強い減速（1 - (1-t)³）
	[[nodiscard]] static double EaseOutCubic(double t) noexcept {
		const double oneMinusT = 1.0 - t;
		return 1.0 - oneMinusT * oneMinusT * oneMinusT;
	}

	// イーズインアウト（三次）。より滑らかな加減速
	[[nodiscard]] static double EaseInOutCubic(double t) noexcept {
		if (t < 0.5) {
			return 4.0 * t * t * t;
		} else {
			const double oneMinusT = 1.0 - t;
			return 1.0 - 4.0 * oneMinusT * oneMinusT * oneMinusT;
		}
	}

	// イーズイン（四次）。さらに強い加速（t⁴）
	[[nodiscard]] static double EaseInQuart(double t) noexcept {
		return t * t * t * t;
	}

	// イーズアウト（四次）。さらに強い減速（1 - (1-t)⁴）
	[[nodiscard]] static double EaseOutQuart(double t) noexcept {
		const double oneMinusT = 1.0 - t;
		return 1.0 - oneMinusT * oneMinusT * oneMinusT * oneMinusT;
	}

	// イーズインアウト（四次）。最も滑らかな加減速
	[[nodiscard]] static double EaseInOutQuart(double t) noexcept {
		if (t < 0.5) {
			return 8.0 * t * t * t * t;
		} else {
			const double oneMinusT = 1.0 - t;
			return 1.0 - 8.0 * oneMinusT * oneMinusT * oneMinusT * oneMinusT;
		}
	}

	// バウンド効果。跳ねるような動き
	[[nodiscard]] static double Bounce(double t) noexcept {
		if (t < 0.36363636) {
			return 7.5625 * t * t;
		} else if (t < 0.72727273) {
			const double t2 = t - 0.54545455;
			return 7.5625 * t2 * t2 + 0.75;
		} else if (t < 0.90909091) {
			const double t2 = t - 0.81818182;
			return 7.5625 * t2 * t2 + 0.9375;
		} else {
			const double t2 = t - 0.95454545;
			return 7.5625 * t2 * t2 + 0.984375;
		}
	}

	// エラスティック効果。弾性のある動き
	[[nodiscard]] static double Elastic(double t) noexcept {
		if (t == 0.0 || t == 1.0) {
			return t;
		}
		constexpr double p = 0.3;
		constexpr double s = p / 4.0;
		const double t1 = t - 1.0;
		return -(Math::Pow(2.0, 10.0 * t1) * Math::Sin((t1 - s) * (2.0 * Math::Pi) / p));
	}
};
