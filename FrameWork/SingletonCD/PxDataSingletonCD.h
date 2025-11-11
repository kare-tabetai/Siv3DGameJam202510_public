#pragma once
#include "flecs/flecs.h"

// MEMO:オブジェクト指向的になってしまっているが、まあいいや
class PxDataSingletonCD : public physx::PxSimulationEventCallback {
  public:
  using OnTriggerHitEvent =
    std::function<void(flecs::entity, flecs::entity, physx::PxTriggerPair&)>;
  using OnCollisionHitEvent = std::function<void(
    flecs::entity, flecs::entity, const physx::PxContactPairHeader&,
    const physx::PxContactPair&)>;

  bool InitializePx() {
    // Foundationのインスタンス化
    if (m_pFoundation = PxCreateFoundation(
          PX_PHYSICS_VERSION, m_defaultAllocator, m_defaultErrorCallback);
        !m_pFoundation) {
      return false;
    }
    // PVDと接続する設定
    if (m_pPvd = physx::PxCreatePvd(*m_pFoundation); m_pPvd) {
      tryConnectPVD(*m_pPvd);
    }
    // Physicsのインスタンス化
    if (m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation,
                                     physx::PxTolerancesScale(), true, m_pPvd);
        !m_pPhysics) {
      return false;
    }
    // 拡張機能用
    if (!PxInitExtensions(*m_pPhysics, m_pPvd)) {
      return false;
    }
    is_ext_initialized = true;
    // 処理に使うスレッドを指定する
    m_pDispatcher = physx::PxDefaultCpuDispatcherCreate(8);
    // 空間の設定
    physx::PxSceneDesc scene_desc(m_pPhysics->getTolerancesScale());
    scene_desc.gravity = physx::PxVec3(0, -9, 0);
    scene_desc.filterShader = FilterShader;
    scene_desc.cpuDispatcher = m_pDispatcher;
    scene_desc.simulationEventCallback = this;
    // 空間のインスタンス化
    m_pScene = m_pPhysics->createScene(scene_desc);
    assert(m_pScene);
    m_pScene->setSimulationEventCallback(this);

    tryInitPVDClient(m_pvd_client, *m_pScene);

    TryCreateAndRegisterMat(kDefaultMatName, 0.6f, 0.6f,
                            0.0f);  // Unityのデフォルトと同じ
    TryCreateAndRegisterMat(kZeroFrictionMatName, 0.0f, 0.0f, 0.0f);

    return true;
  }

  ~PxDataSingletonCD() {
    for (auto& item : m_material_map) {
      item.second->release();
    }

    if (is_ext_initialized) {
      PxCloseExtensions();
    }
    if (m_pScene) {
      m_pScene->release();
    }
    if (m_pDispatcher) {
      m_pDispatcher->release();
    }
    if (m_pPhysics) {
      m_pPhysics->release();
    }
    if (m_pPvd) {
      m_pPvd->disconnect();
      physx::PxPvdTransport* transport = m_pPvd->getTransport();
      m_pPvd->release();
      transport->release();
    }
    if (m_pFoundation) {
      m_pFoundation->release();
    }
  }

  void Update(float dt) {
    if (KeyP.down()) {
      if (m_pPvd) {
        tryConnectPVD(*m_pPvd);

        if (m_pPvd->isConnected()) {
        }
      }
    }

    // シミュレーション速度を指定する
    m_pScene->simulate(dt);
    // PhysXの処理が終わるまで待つ
    m_pScene->fetchResults(true);
  }

  // 本当はラップするべきだが面倒なので露出させている
  physx::PxPhysics* Physics() { return m_pPhysics; }
  bool AddActor(flecs::entity entity, physx::PxActor& actor) {
    m_actor_entity_map.emplace(&actor, entity);
    return m_pScene->addActor(actor);
  }
  void RemoveActor(flecs::entity entity, physx::PxActor& actor) {
    auto itr = m_actor_entity_map.find(&actor);
    assert(itr != m_actor_entity_map.end());
    assert(itr->second == entity);
    m_actor_entity_map.erase(itr);
    m_pScene->removeActor(actor);
  }

  physx::PxMaterial* GetDefaultMat() { return GetMat(kDefaultMatName.data()); }
  physx::PxMaterial* GetZeroMat() {
    return GetMat(kZeroFrictionMatName.data());
  }
  physx::PxMaterial* GetMat(StringView mat_name) {
    assert(m_material_map.contains(mat_name.data()));
    auto mat = m_material_map.find(mat_name.data());
    return mat->second;
  }
  void TryCreateAndRegisterMat(StringView name, float static_frction,
                               float dynamic_friction, float restitution) {
    // 登録済みならやめておく
    if (m_material_map.contains(name.data())) {
      return;
    }

    auto* default_mat =
      Physics()->createMaterial(static_frction, dynamic_friction, restitution);
    m_material_map.emplace(name, default_mat);
  }

  void AddOnTriggerHitEvent(OnTriggerHitEvent&& ev) {
    m_on_trigger_hit_event.push_back(ev);
  }
  void AddOnCollisionHitEvent(OnCollisionHitEvent&& ev) {
    m_on_collision_hit_event.push_back(ev);
  }

  void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override {
    for (physx::PxU32 i = 0; i < count; i++) {
      callOnTriggerHitEvent(pairs[i]);
    }
  }
  void onContact(const physx::PxContactPairHeader& header,
                 const physx::PxContactPair* pairs,
                 physx::PxU32 count) override {
    for (physx::PxU32 i = 0; i < count; i++) {
      callOnCollisionHitEvent(header, pairs[i]);
    }
  }
  void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) override {}
  void onWake(physx::PxActor**, physx::PxU32) override {}
  void onSleep(physx::PxActor**, physx::PxU32) override {}
  void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*,
                 const physx::PxU32) override {}

#pragma warning(push)
#pragma warning(disable : 4100)  // 未使用の仮引数

  // PhysXに登録する関数 onContactの接触設定のために行っている
  static physx::PxFilterFlags FilterShader(
    physx::PxFilterObjectAttributes attributes0,
    physx::PxFilterData filterData0,
    physx::PxFilterObjectAttributes attributes1,
    physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags,
    const void* constantBlock, physx::PxU32 constantBlockSize) {
    // コリジョン検出のペアフラグを設定
    pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT |
                physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
    return physx::PxFilterFlag::eDEFAULT;
  }

#pragma warning(pop)

  private:
  static constexpr StringView kDefaultMatName = U"DefaultMat";
  static constexpr StringView kZeroFrictionMatName = U"ZeroMat";

  void callOnTriggerHitEvent(physx::PxTriggerPair& pair) {
    auto trigger_actor_itr = m_actor_entity_map.find(pair.triggerActor);
    if (trigger_actor_itr == m_actor_entity_map.end()) {
      return;
    }
    auto other_actor_itr = m_actor_entity_map.find(pair.otherActor);
    if (other_actor_itr == m_actor_entity_map.end()) {
      return;
    }

    for (auto& ev : m_on_trigger_hit_event) {
      ev(trigger_actor_itr->second, other_actor_itr->second, pair);
    }
  }

  void callOnCollisionHitEvent(const physx::PxContactPairHeader& header,
                               const physx::PxContactPair& pair) {
    auto hit_actor_itr = m_actor_entity_map.find(header.actors[0]);
    if (hit_actor_itr == m_actor_entity_map.end()) {
      return;
    }
    auto other_hit_actor_itr = m_actor_entity_map.find(header.actors[1]);
    if (other_hit_actor_itr == m_actor_entity_map.end()) {
      return;
    }

    for (auto& ev : m_on_collision_hit_event) {
      ev(hit_actor_itr->second, other_hit_actor_itr->second, header, pair);
    }
  }

  static void tryConnectPVD(physx::PxPvd& pvd) {
    // 接続済みなら帰る
    if (pvd.isConnected()) {
      return;
    }

    // PVD側のデフォルトポートは5425
    physx::PxPvdTransport* transport =
      physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    pvd.connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
  }

  // PVDの表示設定を初期化
  static void tryInitPVDClient(physx::PxPvdSceneClient* pvd_client,
                               physx::PxScene& pScene) {
    pvd_client = pScene.getScenePvdClient();
    if (pvd_client) {
      pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS,
                                  true);
      pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS,
                                  true);
      pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES,
                                  true);
    }
  }

  // PhysX内で利用するアロケーター
  physx::PxDefaultAllocator m_defaultAllocator = {};
  // エラー時用のコールバックでエラー内容が入ってる
  physx::PxDefaultErrorCallback m_defaultErrorCallback = {};
  // 上位レベルのSDK(PxPhysicsなど)をインスタンス化する際に必要
  physx::PxFoundation* m_pFoundation = nullptr;
  // 実際に物理演算を行う
  physx::PxPhysics* m_pPhysics = nullptr;
  // シミュレーションをどう処理するかの設定でマルチスレッドの設定もできる
  physx::PxDefaultCpuDispatcher* m_pDispatcher = nullptr;
  // シミュレーションする空間の単位でActorの追加などもここで行う
  physx::PxScene* m_pScene = nullptr;
  // PVDと通信する際に必要
  physx::PxPvd* m_pPvd = nullptr;
  // PVDに設定を送るために必要
  physx::PxPvdSceneClient* m_pvd_client = nullptr;
  // PVDのカメラを同期していればtrue
  bool is_pvd_camera_sync = false;

  bool is_ext_initialized = false;
  std::unordered_map<String, physx::PxMaterial*> m_material_map = {};
  std::vector<OnTriggerHitEvent> m_on_trigger_hit_event;
  std::vector<OnCollisionHitEvent> m_on_collision_hit_event;
  std::unordered_map<physx::PxActor*, flecs::entity> m_actor_entity_map;
};
