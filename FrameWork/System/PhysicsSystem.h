#pragma once
#include "CD/PositionCD.h"
#include "CD/PxCD.h"
#include "CD/RotationCD.h"
#include "Helper/PxCDHelper.h"
#include "SingletonCD/PxDataSingletonCD.h"
#include "flecs/flecs.h"

class PhysicsSystem {
  public:
  static void Register(flecs::world& world) {
    auto phys_sys = world.system<PxDataSingletonCD>("phys_sys")
                      .kind(flecs::PostUpdate)
                      .each([](PxDataSingletonCD& px) {
                        px.Update(static_cast<float>(Scene::DeltaTime()));
                      });
    auto post_phys_sys =
      world.system<PositionCD, RotationCD, DynamicBodyCD>("post_phys_sys")
        .kind(flecs::PostUpdate)
        .without(flecs::Prefab)
        .each([](PositionCD& pos, RotationCD& rot, DynamicBodyCD& body) {
          auto body_transform = body.rigid->getGlobalPose();
          PxCDHelper::ToPosRot(body_transform, pos, rot);
        });
  }
};
