#include <sputter/game/scene.h>
#include <sputter/game/subsystemprovider.h>
#include <sputter/game/timersystem.h>

// Forward declarations
namespace sputter 
{ 
    namespace assets 
    {
        class AssetStorage;
        class AssetStorageProvider;
    }

    namespace render
    {
        class ShaderStorage;
        class VolumetricTextRenderer;
        class MeshSubsystem;
        class Window;
    }

    namespace physics
    {
        class RigidBodySubsystem;
        class CollisionSubsystem;
    }

    namespace game 
    {
        class SubsystemProvider;
    }

    namespace input
    {
        class InputSubsystem;
    }

    namespace memory
    {
        class FixedMemoryAllocator;
    }
}

struct GameState;

class GameScene : public sputter::game::IScene
{
public:
    GameScene(
        sputter::render::Window* pWindow,
        sputter::game::TimerSystem* pTimerSystem,
        sputter::assets::AssetStorage* pAssetStorage,
        sputter::assets::AssetStorageProvider* pStorageProvider,
        sputter::memory::FixedMemoryAllocator& allocator);

    virtual void Initialize() override;
    virtual void Uninitialize() override;

    virtual void Tick(sputter::math::FixedPoint dt) override;
    virtual void Draw() override;

private:
    void TickFrame(sputter::math::FixedPoint dt);
    void PostTickFrame(sputter::math::FixedPoint dt);

    static void OnCountdownTimerExpired(sputter::game::TimerSystem* pTimerSystem, sputter::game::TimerSystem::TimerHandle handle, void* pUserData);

    sputter::game::SubsystemProvider          m_subsystemProvider;

    sputter::physics::RigidBodySubsystem*     m_pRigidBodySubsystem = nullptr;
    sputter::physics::CollisionSubsystem*     m_pCollisionSubsystem = nullptr;
    sputter::render::MeshSubsystem*           m_pMeshSubsystem = nullptr;
    sputter::input::InputSubsystem*           m_pInputSubsystem = nullptr;

    sputter::assets::AssetStorage*            m_pAssetStorage = nullptr;
    sputter::assets::AssetStorageProvider*    m_pAssetStorageProvider = nullptr;
    sputter::render::ShaderStorage*           m_pShaderStorage = nullptr;

    sputter::render::VolumetricTextRenderer*  m_pTextRenderer = nullptr;

    sputter::game::TimerSystem*               m_pTimerSystem = nullptr;

    GameState*                                m_pGameState = nullptr;
};