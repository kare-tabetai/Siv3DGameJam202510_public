#pragma once
#include <Siv3D.hpp>

#include "CD/ColorCD.h"
#include "CD/FrameCD.h"
#include "CD/MeshCD.h"
#include "CD/PositionCD.h"
#include "CD/PxCD.h"
#include "CD/Rotation2DCD.h"
#include "CD/RotationCD.h"
#include "CD/ScalarScaleCD.h"
#include "CD/Size2DCD.h"
#include "CD/SpriteAnimationCD.h"
#include "CD/TextCD.h"
#include "CD/TextureArrayCD.h"
#include "CD/TextureCD.h"
#include "CD/TimerCD.h"
#include "Helper/PxCDHelper.h"
#include "SharedCD/FontSharedCD.h"
#include "SingletonCD/PxDataSingletonCD.h"
#include "Util/FontUtil.h"
#include "flecs/flecs.h"

class EntityFactory {
  private:
  // インスタンス化を防ぐ
  EntityFactory() = delete;
  EntityFactory(const EntityFactory&) = delete;
  EntityFactory& operator=(const EntityFactory&) = delete;

  // プレハブの保存（static inline変数）
  static inline flecs::entity s_floorPrefab;
  static inline flecs::entity s_staticCubePrefab;
  static inline flecs::entity s_dynamicSpherePrefab;
  static inline flecs::entity s_textUIPrefab;
  static inline flecs::entity s_standardFontPrefab;
  static inline flecs::entity s_assetFontPrefab;
  static inline flecs::entity s_textureSpritePrefab;
  static inline flecs::entity s_spriteAnimationPrefab;
  static inline bool s_initialized = false;

  public:
  // 初期化処理
  static void Initialize(flecs::world& world) {
    if (s_initialized) return;

    // プレハブの作成
    s_floorPrefab = world.prefab("FloorPrefab");
    s_floorPrefab.add<PositionCD>();
    s_floorPrefab.add<RotationCD>();
    s_floorPrefab.add<TextureCD>();
    s_floorPrefab.add<StaticBodyCD>();
    MeshCD model = {Mesh(MeshData::OneSidedPlane(2000, {400, 400}))};
    s_floorPrefab.set<MeshCD>(model);

    s_staticCubePrefab = world.prefab("StaticCubePrefab");
    s_staticCubePrefab.add<PositionCD>();
    s_staticCubePrefab.add<RotationCD>();
    s_staticCubePrefab.add<TextureCD>();
    s_staticCubePrefab.add<MeshCD>();
    s_staticCubePrefab.add<StaticBodyCD>();
    s_staticCubePrefab.add<PxShapeCD>();

    s_dynamicSpherePrefab = world.prefab("DynamicSpherePrefab");
    s_dynamicSpherePrefab.add<PositionCD>();
    s_dynamicSpherePrefab.add<RotationCD>();
    s_dynamicSpherePrefab.add<MeshCD>();
    s_dynamicSpherePrefab.add<ColorCD>();
    s_dynamicSpherePrefab.add<DynamicBodyCD>();
    s_dynamicSpherePrefab.add<PxShapeCD>();

    s_textUIPrefab = world.prefab("TextUIPrefab");
    s_textUIPrefab.add<PositionCD>();
    s_textUIPrefab.add<ScalarScaleCD>();
    s_textUIPrefab.add<TextCD>();
    s_textUIPrefab.add<ColorCD>();

    s_standardFontPrefab = world.prefab("StandardFontPrefab");
    s_standardFontPrefab.add<FontSharedCD>();

    s_assetFontPrefab = world.prefab("AssetFontPrefab");
    s_assetFontPrefab.add<FontSharedCD>();

    s_textureSpritePrefab = world.prefab("TextureSpritePrefab");
    s_textureSpritePrefab.add<PositionCD>();
    s_textureSpritePrefab.add<Rotation2DCD>();
    s_textureSpritePrefab.add<TextureCD>();

    s_spriteAnimationPrefab = world.prefab("SpriteAnimationPrefab");
    s_spriteAnimationPrefab.add<PositionCD>();
    s_spriteAnimationPrefab.add<Rotation2DCD>();
    s_spriteAnimationPrefab.add<ScalarScaleCD>();
    s_spriteAnimationPrefab.add<TextureCD>();
    s_spriteAnimationPrefab.add<SpriteAnimationCD>();
    s_spriteAnimationPrefab.add<TextureArrayCD>();
    s_spriteAnimationPrefab.add<FrameCD>();
    s_spriteAnimationPrefab.add<TimerCD>();

    s_initialized = true;
  }

  static flecs::entity Floor(flecs::world& world, StringView tex_path) {
    auto entity = world.entity("floor");
    entity.is_a(s_floorPrefab);

    TextureCD tex = Texture(tex_path, TextureDesc::MippedSRGB);
    entity.set<TextureCD>(tex);

    assert(world.has<PxDataSingletonCD>());
    PxDataSingletonCD& px = *world.get_ref<PxDataSingletonCD>().get();

    StaticBodyCD body;
    PxCDHelper::CreatePlane(body, px);
    px.AddActor(entity, *body.rigid);
    entity.set<StaticBodyCD>(body);

    return entity;
  }

  static flecs::entity StaticCube(
    flecs::world& world, const Vec3& pos, const FilePath texture_path = U"",
    const Vec3& scale = Vec3::One(),
    const Quaternion& rot = Quaternion::Identity()) {
    auto entity = world.entity();
    entity.is_a(s_staticCubePrefab);
    entity.set<PositionCD>(PositionCD(pos));
    entity.set<RotationCD>(RotationCD(rot));

    if (!texture_path.isEmpty()) {
      TextureCD tex = Texture(texture_path, TextureDesc::MippedSRGB);
      entity.set<TextureCD>(tex);
      MeshCD model = {Mesh(MeshData::Box(scale))};
      entity.set<MeshCD>(model);
    }

    assert(world.has<PxDataSingletonCD>());
    PxDataSingletonCD& px = *world.get_ref<PxDataSingletonCD>().get();

    StaticBodyCD body;
    PxShapeCD shape;
    auto px_scale = PxUtil::ToVec(scale / 2.0);
    physx::PxShape* box = px.Physics()->createShape(
      physx::PxBoxGeometry(px_scale), *px.GetDefaultMat());
    PxCDHelper::InitStatic(px, body, shape, box);
    body.rigid->setGlobalPose(PxCDHelper::ToTransform(
      entity.get<PositionCD>(), entity.get<RotationCD>()));
    px.AddActor(entity, *body.rigid);
    entity.set<PxShapeCD>(shape);
    entity.set<StaticBodyCD>(body);

    return entity;
  }

  static flecs::entity DynamicSphere(flecs::world& world, const Vec3& pos,
                                     float radius = 0.5f,
                                     const Color& color = Palette::White) {
    auto entity = world.entity();
    entity.is_a(s_dynamicSpherePrefab);
    entity.set<PositionCD>(PositionCD(pos));

    MeshCD model = {Mesh(MeshData::Sphere(radius))};
    entity.set<MeshCD>(model);
    entity.set<ColorCD>(ColorCD(color));

    assert(world.has<PxDataSingletonCD>());
    PxDataSingletonCD& px = *world.get_ref<PxDataSingletonCD>().get();

    DynamicBodyCD body;
    PxShapeCD shape;
    physx::PxShape* sphere = px.Physics()->createShape(
      physx::PxSphereGeometry(radius), *px.GetDefaultMat());
    PxCDHelper::InitDynamic(px, body, shape, sphere);
    const auto* pos_cd = entity.try_get<PositionCD>();
    const auto* rot_cd = entity.try_get<RotationCD>();
    assert(pos_cd && rot_cd);
    body.rigid->setGlobalPose(PxCDHelper::ToTransform(*pos_cd, *rot_cd));
    px.AddActor(entity, *body.rigid);
    entity.set<PxShapeCD>(shape);
    entity.set<DynamicBodyCD>(body);

    return entity;
  }

  static flecs::entity StandardFont(flecs::world& world, int32 font_size = 48,
                                    FontMethod font_method = FontMethod::MSDF,
                                    Typeface typeface = Typeface::Regular) {
    auto entity = world.entity();
    entity.is_a(s_standardFontPrefab);

    FontSharedCD font;
    font.name = FontUtil::StandardFontName;
    font.font = Font(font_method, font_size, typeface);
    entity.set<FontSharedCD>(font);
    return entity;
  }

  static flecs::entity AssetFont(flecs::world& world, StringView name,
                                 StringView font_path, int32 font_size = 48,
                                 FontStyle font_method = FontStyle::Default) {
    auto entity = world.entity();
    entity.is_a(s_assetFontPrefab);

    FontSharedCD font;
    font.name = String(name);
    font.font = Font(font_size, font_path, font_method);
    entity.set<FontSharedCD>(font);
    return entity;
  }

  static flecs::entity TextUI(flecs::world& world, flecs::entity font_entity,
                              StringView text, float font_size,
                              const Vec2& pos = Vec2(0, 0),
                              const Color& color = Palette::White) {
    auto entity = world.entity();
    entity.is_a(s_textUIPrefab);
    entity.is_a(font_entity);
    entity.set<PositionCD>(PositionCD({pos.x, pos.y, 0.0}));
    entity.set<ScalarScaleCD>({font_size});
    entity.set<TextCD>(TextCD({String(text)}));
    entity.set<ColorCD>(color);
    return entity;
  }

  static flecs::entity EmojiSprite(flecs::world& world, const Emoji& emoji,
                                   const Vec3& pos = Vec3::Zero(),
                                   double rotation = 0.0, float scale = 1.0f) {
    auto entity = world.entity();
    entity.is_a(s_textureSpritePrefab);

    entity.set<PositionCD>(PositionCD(pos));
    entity.set<Rotation2DCD>(Rotation2DCD(rotation));
    entity.set<ScalarScaleCD>(ScalarScaleCD(scale));

    TextureCD texture = Texture(emoji, TextureDesc::MippedSRGB);
    entity.set<TextureCD>(texture);

    return entity;
  }

  static flecs::entity TextureSprite(flecs::world& world,
                                     StringView texture_path,
                                     const Vec3& pos = Vec3::Zero(),
                                     double rotation = 0.0,
                                     float scale = 1.0f) {
    auto entity = world.entity();
    entity.is_a(s_textureSpritePrefab);

    entity.set<PositionCD>(PositionCD(pos));
    entity.set<Rotation2DCD>(Rotation2DCD(rotation));
    entity.set<ScalarScaleCD>(ScalarScaleCD(scale));

    TextureCD texture = Texture(texture_path, TextureDesc::MippedSRGB);
    entity.set<TextureCD>(texture);

    return entity;
  }

  // Size2DCDを使用するスプライト作成関数
  static flecs::entity TextureSpriteWithSize(flecs::world& world,
                                             StringView texture_path,
                                             const Vec3& pos, double rotation,
                                             const Vec2& size) {
    auto entity = world.entity();
    entity.is_a(s_textureSpritePrefab);

    entity.set<PositionCD>(PositionCD(pos));
    entity.set<Rotation2DCD>(Rotation2DCD(rotation));
    entity.set<Size2DCD>(Size2DCD(size));

    TextureCD texture = Texture(texture_path, TextureDesc::MippedSRGB);
    entity.set<TextureCD>(texture);

    return entity;
  }

  // スプライトアニメーション用エンティティ作成関数
  static flecs::entity SpriteAnimation(
    flecs::world& world, const std::vector<FilePath>& texture_paths,
    double interval = 0.2, bool loop = true, const Vec3& pos = Vec3::Zero(),
    double rotation = 0.0, float scale = 1.0f) {
    auto entity = world.entity();
    entity.is_a(s_spriteAnimationPrefab);

    // Transform系コンポーネントを設定
    entity.set<PositionCD>(PositionCD(pos));
    entity.set<Rotation2DCD>(Rotation2DCD(rotation));
    entity.set<ScalarScaleCD>(ScalarScaleCD(scale));

    // テクスチャ配列を作成
    std::vector<Texture> textures;
    textures.reserve(texture_paths.size());

    for (const auto& path : texture_paths) {
      textures.emplace_back(Texture(path, TextureDesc::MippedSRGB));
    }

    // アニメーション関連コンポーネントを設定
    entity.set<SpriteAnimationCD>(SpriteAnimationCD(interval, loop));
    entity.set<TextureArrayCD>(TextureArrayCD(textures));
    entity.set<FrameCD>(FrameCD(0));
    entity.set<TimerCD>(TimerCD(0.0));

    // 最初のフレームのテクスチャを設定
    auto* tex_cd = entity.try_get_mut<TextureCD>();
    if (tex_cd) {
      tex_cd->tex = textures.front();
    }

    return entity;
  }

};  // class EntityFactory
