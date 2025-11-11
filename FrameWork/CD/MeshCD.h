#pragma once
#include <Siv3D.hpp>

struct MeshCD {
  MeshCD() = default;
  MeshCD(Mesh&& m) : mesh(m) {}

  Mesh mesh;
};
