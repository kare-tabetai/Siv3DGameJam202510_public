#pragma once
#include "flecs/flecs.h"

// 子エンティティを表すコンポーネント
// 親エンティティを一つだけ持つ
struct ChildCD {
  ChildCD() = default;
  ChildCD(flecs::entity p) : parent(p) {}

  flecs::entity parent;
};
