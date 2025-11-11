#pragma once
#include <Siv3D.hpp>

struct CameraSingletonCD {
  DebugCamera3D camera =
    DebugCamera3D(Graphics3D::GetRenderTargetSize(), 40_deg, Vec3(0, 26, -23),
                  Vec3(0, 24.8f, -22));
  MSRenderTexture rt = MSRenderTexture(
    Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes);

  Optional<Optional<RenderTexture>> old_rt = none;
  Optional<Optional<Rect>> old_vp = none;

  bool update_enable = false;
};
