#pragma once
#include "CD/PxCD.h"
#include "SingletonCD/PxDataSingletonCD.h"

namespace PxCDHelper {

// Body系CDが削除されたときPhysXのActorを削除する処理を設定
// MEMO:PxDataSingletonCD::Initializeに含めようとすると循環includeが発生するため、ここに
void InitializeRemoveBodyObserver(flecs::world& world) {
  world.observer<DynamicBodyCD>()
    .event(flecs::OnRemove)
    .each([&world](flecs::entity e, DynamicBodyCD& p) {
      auto px_data = world.get_ref<PxDataSingletonCD>();
      px_data->RemoveActor(e, *p.rigid);
      p.rigid->release();
    });
  world.observer<StaticBodyCD>()
    .event(flecs::OnRemove)
    .each([&world](flecs::entity e, StaticBodyCD& p) {
      auto px_data = world.get_ref<PxDataSingletonCD>();
      px_data->RemoveActor(e, *p.rigid);
      p.rigid->release();
    });
}

void InitStatic(PxDataSingletonCD& px, StaticBodyCD& s_body_cd,
                PxShapeCD& shape_cd, physx::PxShape* shape) {
  s_body_cd.rigid =
    px.Physics()->createRigidStatic(physx::PxTransform(physx::PxIdentity));
  shape_cd.shape = shape;
  shape_cd.shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
  s_body_cd.rigid->attachShape(*shape_cd.shape);
}

void InitDynamic(PxDataSingletonCD& px, DynamicBodyCD& d_body_cd,
                 PxShapeCD& shape_cd, physx::PxShape* shape) {
  d_body_cd.rigid =
    px.Physics()->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
  shape_cd.shape = shape;
  shape_cd.shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
  d_body_cd.rigid->attachShape(*shape_cd.shape);
}

void ReleaseStatic(StaticBodyCD& body_cd) {
  body_cd.rigid->release();
  body_cd.rigid = nullptr;
}

void ReleaseDynamic(DynamicBodyCD& body_cd) {
  body_cd.rigid->release();
  body_cd.rigid = nullptr;
}

void CreatePlane(StaticBodyCD& s_body_cd, PxDataSingletonCD& px) {
  s_body_cd.rigid = physx::PxCreatePlane(
    *px.Physics(), physx::PxPlane(0, 1, 0, 0), *px.GetDefaultMat());
}

physx::PxTransform ToTransform(const PositionCD& pos_cd,
                               const RotationCD& rot_cd) {
  return PxUtil::ToTransform(pos_cd.pos, rot_cd.rot);
}

void ToPosRot(const physx::PxTransform& transform, PositionCD& pos_cd,
              RotationCD& rot_cd) {
  pos_cd.pos = Vec3(transform.p.x, transform.p.y, transform.p.z);
  rot_cd.rot =
    Quaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w);
}

}  // namespace PxCDHelper
