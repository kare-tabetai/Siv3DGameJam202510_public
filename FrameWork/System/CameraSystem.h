#pragma once
#include "SingletonCD/CameraSingletonCD.h"
#include "flecs/flecs.h"

class CameraSystem {
  public:
  static void Register(flecs::world& world) {
    auto camera_pre_frame_sys =
      world.system<CameraSingletonCD>("camera_pre_frame_sys")
        .kind(flecs::PreFrame)
        .each([](CameraSingletonCD& camera) {
          if (KeyL.down()) {
            camera.update_enable = !camera.update_enable;
          }
          if (camera.update_enable) {
            camera.camera.update(4.0);
          }
          Graphics3D::SetCameraTransform(camera.camera);

          const ColorF backgroundColor =
            ColorF{0.4, 0.6, 0.8}.removeSRGBCurve();
          camera.rt.clear(backgroundColor);
          camera.old_rt = Graphics3D::GetRenderTarget();
          camera.old_vp = Graphics3D::GetViewport();
          s3d::Graphics3D::Internal::SetRenderTarget(camera.rt);
          Graphics3D::Internal::SetViewport(Rect{camera.rt.size()});
        });

    auto camera_post_frame_sys =
      world.system<CameraSingletonCD>("camera_post_frame_sys")
        .kind(flecs::OnStore)
        .each([](CameraSingletonCD& camera) {
          camera.old_rt.then(Graphics3D::Internal::SetRenderTarget);
          camera.old_vp.then(Graphics3D::Internal::SetViewport);

          Graphics3D::Flush();
          camera.rt.resolve();
          Shader::LinearToScreen(camera.rt);
        });
  }
};
