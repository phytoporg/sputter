#pragma once

#include "gamemode.h"

#include <sputter/memory/fixedmemoryallocator.h>

#include <sputter/game/game.h>
#include <sputter/game/scenestack.h>
#include <sputter/game/subsystemprovider.h>
#include <sputter/game/timersystem.h>

#include <sputter/assets/assetstorage.h>
#include <sputter/assets/assetstorageprovider.h>

#include <sputter/render/shaderstorage.h>
#include <sputter/render/fontstorage.h>
#include <sputter/render/camera.h>

#include <sputter/math/fixedpoint.h>

#include <glm/glm.hpp>

#include <string>

// Forward declarations
namespace sputter 
{
    namespace render 
    {
        class VolumetricTextRenderer;
        class Window;
    }

    namespace net
    {
        class ReliableUDPSession;
    }
}

enum class PaddleArenaInput {
    // Starting out with forreal pong
    INPUT_MOVE_UP,
    INPUT_MOVE_DOWN,
    INPUT_MOVE_LEFT,
    INPUT_MOVE_RIGHT,
    INPUT_PAUSE,
    INPUT_SERVE, // Can also be used for other things though??
    INPUT_DASH,
};

class PaddleArena : public sputter::game::Game
{
public:
    // Address and port are only interesting for the client game mode
    PaddleArena(
        sputter::render::Window* pWindow,
        const std::string& assetStoragePath,
        GameMode gameMode,
        const std::string& remoteServerAddress,
        int32_t remoteServerPort);
    virtual ~PaddleArena();

    virtual void Tick(sputter::math::FixedPoint deltaTime) override;
    virtual void Draw() override;
    virtual bool StartGame() override;

    void NextSceneFromMainMenu();
    void PreviousSceneFromGame();

    void NextSceneFromP2PScreen();

    GameMode GetGameMode() const;
    const char* GetRemoteServerAddress() const;
    int32_t GetRemoteServerPort() const;
    sputter::net::ReliableUDPSession* GetUDPSession() const;
    void SetUDPSession(sputter::net::ReliableUDPSession* pSession);

private:
    PaddleArena() = delete;
    PaddleArena(const PaddleArena& other) = delete;

    GameMode                                 m_gameMode = GameMode::Local;
    const std::string                        m_remoteServerAddress;
    const int32_t                            m_remoteServerPort;

    sputter::game::IScene*                   m_pMainMenuScene = nullptr;
    sputter::game::IScene*                   m_pGameScene = nullptr;
    sputter::game::IScene*                   m_pP2PScene = nullptr;
    sputter::game::SceneStack*               m_pSceneStack = nullptr;

    sputter::net::ReliableUDPSession*        m_pReliableUDPSession = nullptr;

    sputter::game::TimerSystem               m_timerSystem;

    sputter::assets::AssetStorage            m_assetStorage;
    sputter::render::ShaderStorage           m_shaderStorage;
    sputter::render::FontStorage             m_fontStorage;

    sputter::render::Camera                  m_camera;
    glm::mat4                                m_orthoMatrix;

    sputter::assets::AssetStorageProvider    m_storageProvider;
    sputter::game::SubsystemProvider         m_subsystemProvider;

    sputter::render::Window*                 m_pWindow = nullptr;
    sputter::render::VolumetricTextRenderer* m_pTextRenderer = nullptr;
};
