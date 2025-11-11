#pragma once
#include "CD/ColorCD.h"
#include "CD/MeshCD.h"
#include "CD/ModelCD.h"
#include "CD/PositionCD.h"
#include "CD/RotationCD.h"
#include "CD/TextureCD.h"
#include "flecs/flecs.h"

class RenderingSystem {
  public:
  static void Register(flecs::world& world) {
    auto color_mesh_draw_sys =
      world.system<PositionCD, RotationCD, MeshCD, ColorCD>("mesh_draw_sys")
        .kind(flecs::PreStore)
        .without(flecs::Prefab)
        .each(
          [](PositionCD& pos, RotationCD& rot, MeshCD& mesh, ColorCD& color) {
            mesh.mesh.draw(pos.pos, rot.rot, color.c);
          });

    auto tex_mesh_draw_sys =
      world
        .system<PositionCD, RotationCD, MeshCD, TextureCD>("tex_mesh_draw_sys")
        .kind(flecs::PreStore)
        .without(flecs::Prefab)
        .each(
          [](PositionCD& pos, RotationCD& rot, MeshCD& mesh, TextureCD& tex) {
            mesh.mesh.draw(pos.pos, rot.rot, tex.tex);
          });

    auto model_draw_sys =
      world.system<PositionCD, RotationCD, ModelCD>("model_draw_sys")
        .kind(flecs::PreStore)
        .without(flecs::Prefab)
        .each([](PositionCD& pos, RotationCD& rot, ModelCD& model) {
          model.model.draw(pos.pos, rot.rot);
        });
  }
};
