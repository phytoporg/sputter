#pragma once

#include <sputter/memory/fixedmemoryallocator.h>

#include <sputter/game/game.h>
#include <sputter/game/scenestack.h>
#include <sputter/game/subsystemprovider.h>
#include <sputter/game/timersystem.h>

#include <sputter/assets/assetstorage.h>
#include <sputter/assets/assetstorageprovider.h>

#include <sputter/render/shaderstorage.h>
#include <sputter/render/fontstorage.h>

#include <sputter/math/fixedpoint.h>

#include <string>

// Forward declarations
namespace sputter 
{
    namespace render 
    {
        class VolumetricTextRenderer;
        class Window;
    }
}

enum class PaddleArenaInput {
    // Starting out with forreal pong
    INPUT_MOVE_UP,
    INPUT_MOVE_DOWN,
    INPUT_MOVE_LEFT,
    INPUT_MOVE_RIGHT
};

class PaddleArena : public sputter::game::Game
{
public:
    PaddleArena(
        sputter::render::Window* pWindow,
        const std::string& assetStoragePath,
        sputter::memory::FixedMemoryAllocator allocator);
    virtual ~PaddleArena();

    virtual void Tick(sputter::math::FixedPoint deltaTime) override;
    virtual void Draw() override;
    virtual bool StartGame() override;

private:
    PaddleArena() = delete;
    PaddleArena(const PaddleArena& other) = delete;

    sputter::game::SceneStack*               m_pSceneStack = nullptr;

    sputter::game::TimerSystem               m_timerSystem;

    sputter::assets::AssetStorage            m_assetStorage;
    sputter::render::ShaderStorage           m_shaderStorage;
    sputter::render::FontStorage             m_fontStorage;

    sputter::assets::AssetStorageProvider    m_storageProvider;
    sputter::game::SubsystemProvider         m_subsystemProvider;

    sputter::render::Window*                 m_pWindow = nullptr;
    sputter::render::VolumetricTextRenderer* m_pTextRenderer = nullptr;
};
