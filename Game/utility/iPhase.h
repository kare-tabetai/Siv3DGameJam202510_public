// iPhase.h
#pragma once
#include <Siv3D.hpp>

class iPhase {
public:
    virtual ~iPhase() = default;

    // 更新処理
    virtual void update() = 0;

    // 描画処理
    virtual void draw() const = 0;

    // 暗転の手前に描画するものがあればここで描画
    virtual void postDraw() const {}
};
