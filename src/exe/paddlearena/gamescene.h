#pragma once

#include <sputter/game/scene.h>
#include <sputter/game/subsystemprovider.h>
#include <sputter/game/timersystem.h>

#include <sputter/ui/screen.h>
#include <sputter/ui/theme.h>

#include <sputter/memory/reservedregion.h>
#include <sputter/memory/fixedmemoryallocator.h>

#include <glm/glm.hpp>

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
        class Camera;
        class MeshSubsystem;
        class ShaderStorage;
        class VolumetricTextRenderer;
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
        class InputSource;
    }

    namespace ui
    {
        class ModalPopup;
    }

    namespace memory
    {
        class FixedMemoryAllocator;
    }
}

struct GameState;
class PaddleArena;

class GameScene : public sputter::game::IScene
{
public:
    GameScene(
        sputter::render::Window* pWindow,
        PaddleArena* pPaddleArena,
        sputter::game::TimerSystem* pTimerSystem,
        sputter::render::Camera* pCamera,
        glm::mat4* pOrthoMatrix,
        sputter::render::VolumetricTextRenderer* pTextRenderer,
        sputter::assets::AssetStorage* pAssetStorage,
        sputter::assets::AssetStorageProvider* pStorageProvider);

    virtual ~GameScene();

    virtual void Initialize() override;
    virtual void Uninitialize() override;

    void CreateEndOfGameModalPopup();
    void DestroyEndOfGameModalPopup();

    virtual void Tick(sputter::math::FixedPoint dt) override;
    virtual void Draw() override;

private:
    void TickFrame(sputter::math::FixedPoint dt);
    void PostTickFrame(sputter::math::FixedPoint dt);

    bool CheckPauseInput() const;

    static void OnCountdownTimerExpired(sputter::game::TimerSystem* pTimerSystem, sputter::game::TimerSystem::TimerHandle handle, void* pUserData);

    sputter::memory::ReservedRegion           m_reservedRegion;
    sputter::memory::FixedMemoryAllocator     m_fixedAllocator;

    sputter::game::SubsystemProvider          m_subsystemProvider;

    sputter::physics::RigidBodySubsystem*     m_pRigidBodySubsystem = nullptr;
    sputter::physics::CollisionSubsystem*     m_pCollisionSubsystem = nullptr;
    sputter::render::MeshSubsystem*           m_pMeshSubsystem = nullptr;
    sputter::input::InputSubsystem*           m_pInputSubsystem = nullptr;
    const sputter::input::InputSource*        m_pInputSources[2] = {};

    sputter::assets::AssetStorage*            m_pAssetStorage = nullptr;
    sputter::assets::AssetStorageProvider*    m_pAssetStorageProvider = nullptr;
    sputter::render::ShaderStorage*           m_pShaderStorage = nullptr;

    sputter::render::Camera*                  m_pCamera = nullptr;
    glm::mat4*                                m_pOrthoMatrix = nullptr;

    sputter::render::VolumetricTextRenderer*  m_pTextRenderer = nullptr;
    sputter::render::Window*                  m_pWindow = nullptr;
    sputter::ui::Screen*                      m_pScreen = nullptr;
    sputter::ui::ModalPopup*                  m_pModalPopup = nullptr;
    sputter::ui::Theme                        m_uiTheme;

    sputter::game::TimerSystem*               m_pTimerSystem = nullptr;

    GameState*                                m_pGameState = nullptr;
    PaddleArena*                              m_pPaddleArena = nullptr;
};