#pragma once

#include <sputter/game/scene.h>
#include <sputter/game/subsystemprovider.h>
#include <sputter/game/timersystem.h>

#include <sputter/core/serializedframestorage.h>

#include <sputter/memory/reservedregion.h>
#include <sputter/memory/fixedmemoryallocator.h>

#include <sputter/ui/screen.h>
#include <sputter/ui/theme.h>

#include <glm/glm.hpp>

#include "localgametickdriver.h"

// Forward declarations
namespace sputter 
{ 
    namespace assets 
    {
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
class GameInstance;

class GameScene : public sputter::game::IScene
{
public:
    GameScene(
        sputter::render::Window* pWindow,
        PaddleArena* pPaddleArena,
        sputter::render::Camera* pCamera,
        glm::mat4* pOrthoMatrix,
        sputter::render::VolumetricTextRenderer* pTextRenderer,
        sputter::assets::AssetStorageProvider* pStorageProvider);

    virtual ~GameScene();

    virtual void Initialize() override;
    virtual void Uninitialize() override;

    void CreateEndOfGameModalPopup();
    void DestroyEndOfGameModalPopup();

    virtual void Tick(sputter::math::FixedPoint dt) override;
    virtual void Draw() override;

private:
    sputter::memory::ReservedRegion           m_reservedRegion;
    sputter::memory::FixedMemoryAllocator     m_fixedAllocator;

    sputter::game::SubsystemProvider          m_subsystemProvider;
    sputter::game::TimerSystem                m_timerSystem;

    sputter::input::InputSubsystem*           m_pInputSubsystem = nullptr;
    sputter::input::InputSource*              m_pInputSources[2] = {};

    sputter::assets::AssetStorageProvider*    m_pAssetStorageProvider = nullptr;

    sputter::render::Camera*                  m_pCamera = nullptr;
    glm::mat4*                                m_pOrthoMatrix = nullptr;

    sputter::render::VolumetricTextRenderer*  m_pTextRenderer = nullptr;
    sputter::render::Window*                  m_pWindow = nullptr;
    sputter::ui::Screen*                      m_pScreen = nullptr;
    sputter::ui::ModalPopup*                  m_pModalPopup = nullptr;
    sputter::ui::Theme                        m_uiTheme;

    GameState*                                m_pGameState = nullptr;
    PaddleArena*                              m_pPaddleArena = nullptr;

    GameInstance*                             m_pGameInstance = nullptr;

    LocalGameTickDriver*                      m_pGameTickDriver = nullptr;
};
