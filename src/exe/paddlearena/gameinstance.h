#pragma once

#include "gamestate.h"

#include <sputter/core/serializable.h>
#include <sputter/math/fixedpoint.h>
#include <glm/glm.hpp>

#include <cstdint>
#include <functional>

namespace sputter
{
    namespace assets
    {
        class AssetStorageProvider;
    }

    namespace game 
    {
        class TimerSystem;
        class SubsystemProvider;
    }

    namespace render
    {
        class Camera;
        class VolumetricTextRenderer;
    }

    namespace physics
    {
        class RigidBodySubsystem;
        class CollisionSubsystem;
    }

    namespace input
    {
        class InputSubsystem;
        class InputSource;
    }

    namespace memory
    {
        class FixedMemoryAllocator;
    }
}

class GameInstance : public sputter::core::ISerializable {
public:
    GameInstance(
        sputter::memory::FixedMemoryAllocator* pAllocator,
        sputter::assets::AssetStorageProvider* pAssetStorageProvider,
        sputter::game::SubsystemProvider* pSubsystemProvider,
        sputter::game::TimerSystem* pTimerSystem,
        sputter::render::Camera* pCamera,
        glm::mat4* pOrthoMatrix,
        sputter::render::VolumetricTextRenderer* pTextRenderer,
        sputter::input::InputSource* pP1InputSource,
        sputter::input::InputSource* pP2InputSource
    );

    // ++ISerializable
    virtual bool Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes) override;
    virtual bool Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes) override;
    // --ISerializable

    void Initialize();

    void Restart();
    void Exit();

    void Tick(sputter::math::FixedPoint dt, uint32_t p1InputState, uint32_t p2InputState);
    void PostTick(sputter::math::FixedPoint dt);

    uint32_t GetFrame() const;

    void Draw();

    void GetCurrentScore(int16_t* pP1ScoreOut, int16_t* pP2ScoreOut);

    typedef std::function<void(GameState::State)> GameStateChangedCallback;
    void SetGameStateChangedCallback(GameStateChangedCallback fnOnGameStateChanged);

private:
    void SetGameState(GameState::State newState);
    bool CheckPauseInput() const;

    static void OnCountdownTimerExpired(void *pUserData);

    GameStateChangedCallback                 m_fnOngameStateChanged = nullptr;

    sputter::game::TimerSystem*              m_pTimerSystem  = nullptr;
    sputter::render::Camera*                 m_pCamera       = nullptr;
    glm::mat4*                               m_pOrthoMatrix  = nullptr;
    sputter::render::VolumetricTextRenderer* m_pTextRenderer = nullptr;

    sputter::game::SubsystemProvider*        m_pSubsystemProvider = nullptr;
    sputter::assets::AssetStorageProvider*   m_pAssetStorageProvider = nullptr;

    sputter::physics::RigidBodySubsystem*    m_pRigidBodySubsystem = nullptr;
    sputter::physics::CollisionSubsystem*    m_pCollisionSubsystem = nullptr;
    sputter::render::MeshSubsystem*          m_pMeshSubsystem = nullptr;
    sputter::input::InputSubsystem*          m_pInputSubsystem = nullptr;
    sputter::input::InputSource*             m_pInputSources[2] = {};

    GameState*                               m_pGameState    = nullptr;
};