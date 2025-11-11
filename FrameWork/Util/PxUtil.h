#pragma once
#include "foundation/PxTransform.h"

namespace PxUtil {

physx::PxTransform ToTransform(const Vec3& pos,
                               const Quaternion& rot = Quaternion::Identity()) {
  physx::PxVec3 px_pos(static_cast<float>(pos.x), static_cast<float>(pos.y),
                       static_cast<float>(pos.z));
  physx::PxQuat px_rot(rot.getX(), rot.getY(), rot.getZ(), rot.getW());
  return physx::PxTransform(px_pos, px_rot);
}
physx::PxTransform ToTransform(double x, double y, double z,
                               const Quaternion& rot = Quaternion::Identity()) {
  return PxUtil::ToTransform(Vec3(x, y, z), rot);
}
physx::PxVec3 ToVec(const Vec3& vec) {
  return physx::PxVec3(static_cast<float>(vec.x), static_cast<float>(vec.y),
                       static_cast<float>(vec.z));
}

Vec3 ToSivVec(const physx::PxVec3& vec) { return Vec3(vec.x, vec.y, vec.z); }

void SetTrigger(physx::PxShape& shape) {
  shape.setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
  shape.setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
}

}  // namespace PxUtil
