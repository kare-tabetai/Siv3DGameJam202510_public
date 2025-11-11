#pragma once
#include <functional>

#include "PxPhysicsAPI.h"
#include "Util/PxUtil.h"
#include "flecs/flecs.h"

struct StaticBodyCD {
  physx::PxRigidStatic* rigid = nullptr;
};

struct DynamicBodyCD {
  physx::PxRigidDynamic* rigid = nullptr;
};

struct PxShapeCD {
  physx::PxShape* shape = nullptr;
};

// トリガーCD - 物理トリガーとして機能するコンポーネント
struct TriggerCD {
  // トリガーが有効かどうか
  bool isEnabled = true;

  // トリガー発火時に実行するコールバック関数
  // 引数: トリガーに接触したエンティティのID
  std::function<void(flecs::entity)> onTriggerEnter = nullptr;

  // トリガーから離脱した時に実行するコールバック関数
  // 引数: トリガーから離脱したエンティティのID
  std::function<void(flecs::entity)> onTriggerExit = nullptr;

  // トリガーが継続的に接触している時に実行するコールバック関数
  // 引数: トリガーに接触しているエンティティのID
  std::function<void(flecs::entity)> onTriggerStay = nullptr;
};
