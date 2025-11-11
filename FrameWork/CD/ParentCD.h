#pragma once
#include <vector>

#include "flecs/flecs.h"

// 親エンティティを表すコンポーネント
// 複数の子エンティティを持つことができる
struct ParentCD {
  ParentCD() = default;
  ParentCD(const std::vector<flecs::entity>& c) : children(c) {}
  ParentCD(std::vector<flecs::entity>&& c) : children(std::move(c)) {}

  std::vector<flecs::entity> children;
};
