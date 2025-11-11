#pragma once

#include <array>
#include <Siv3D.hpp>

enum class PhaseType {
    Introduction,
    Sunrise,
    JobSearch,
    SisterMessage,
    Work,
    Sunset,
    NightDream,
    GameOver,
    BadEnd,
    GameClear
};

// 名前配列とユーティリティ関数
namespace phase_type_util {
    constexpr int Count = static_cast<int>(PhaseType::GameClear) + 1;

    // 列挙値に対応する表示名（StringView）
    inline constexpr std::array<StringView, Count> Names = {
        U"Introduction",
        U"Sunrise",
        U"JobSearch",
        U"SisterMessage",
        U"Work",
        U"Sunset",
        U"NightDream",
        U"GameOver",
        U"BadEnd",
        U"GameClear",
    };

    [[nodiscard]] inline StringView ToStringView(PhaseType p) noexcept {
        const int idx = static_cast<int>(p);
        if (idx < 0 || idx >= Count) {
            return U"Unknown";
        }
        return Names[idx];
    }

    [[nodiscard]] inline String ToString(PhaseType p) {
        return String(ToStringView(p));
    }
} // namespace phase_type_util

