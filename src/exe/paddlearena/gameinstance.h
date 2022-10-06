#pragma once

#include "gamestate.h"

#include <sputter/math/fixedpoint.h>
#include <glm/glm.hpp>

#include <cstdint>
#include <functional>

namespace sputter
{
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

class GameInstance {
public:
    GameInstance(
        sputter::memory::FixedMemoryAllocator* pAllocator,
        sputter::game::SubsystemProvider* pSubsystemProvider,
        GameState* pGameState,
        sputter::game::TimerSystem* pTimerSystem,
        sputter::render::Camera* pCamera,
        glm::mat4* pOrthoMatrix,
        sputter::render::VolumetricTextRenderer* pTextRenderer,
        const sputter::input::InputSource* pP1InputSource,
        const sputter::input::InputSource* pP2InputSource
    );

    void Initialize();

    void Restart();
    void Exit();

    void Tick(sputter::math::FixedPoint dt);
    void Draw();

    typedef std::function<void(GameState::State)> GameStateChangedCallback;
    void SetGameStateChangedCallback(GameStateChangedCallback fnOnGameStateChanged);

private:
    void SetGameState(GameState::State newState);

    GameStateChangedCallback                 m_fnOngameStateChanged = nullptr;

    sputter::game::TimerSystem*              m_pTimerSystem  = nullptr;
    sputter::render::Camera*                 m_pCamera       = nullptr;
    glm::mat4*                               m_pOrthoMatrix  = nullptr;
    sputter::render::VolumetricTextRenderer* m_pTextRenderer = nullptr;

    sputter::game::SubsystemProvider*        m_pSubsystemProvider = nullptr;

    sputter::physics::RigidBodySubsystem*    m_pRigidBodySubsystem = nullptr;
    sputter::physics::CollisionSubsystem*    m_pCollisionSubsystem = nullptr;
    sputter::render::MeshSubsystem*          m_pMeshSubsystem = nullptr;
    sputter::input::InputSubsystem*          m_pInputSubsystem = nullptr;
    const sputter::input::InputSource*       m_pInputSources[2] = {};

    GameState*                               m_pGameState    = nullptr;
};