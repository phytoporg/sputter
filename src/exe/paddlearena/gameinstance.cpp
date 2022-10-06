#include "gameinstance.h"
#include "gameconstants.h"

#include <sputter/game/timersystem.h>

#include <sputter/physics/rigidbodysubsystem.h>
#include <sputter/physics/collisionsubsystem.h>

#include <sputter/render/meshsubsystem.h>
#include <sputter/render/volumetrictext.h>

#include <sputter/memory/fixedmemoryallocator.h>

namespace {
    void DrawScore(int x, int y, sputter::render::VolumetricTextRenderer* pTextRenderer, uint16_t score)
    {
        // itoa for unsigned shorts, more or less. Scores shouldn't ever get to four
        // digits, but hey, just in case.
        char scoreBuffer[5] = {}; // +1 for null termination
        char* pScoreString = &scoreBuffer[sizeof(scoreBuffer) - 1];
        do
        {
            pScoreString--;
            *pScoreString = '0' + (score % 10);
            score /= 10;
        } while (score && (pScoreString > scoreBuffer));

        pTextRenderer->DrawText(x, y, gameconstants::ScoreSize, pScoreString);
    }
}

using namespace sputter;


GameInstance::GameInstance(
    memory::FixedMemoryAllocator* pAllocator,
    game::SubsystemProvider* pSubsystemProvider,
    GameState* pGameState,
    game::TimerSystem* pTimerSystem,
    render::Camera* pCamera,
    glm::mat4* pOrthoMatrix,
    render::VolumetricTextRenderer* pTextRenderer, 
    const sputter::input::InputSource* pP1InputSource,
    const sputter::input::InputSource* pP2InputSource)
    : m_pGameState(pGameState), 
      m_pTimerSystem(pTimerSystem),
      m_pCamera(pCamera),
      m_pOrthoMatrix(pOrthoMatrix),
      m_pTextRenderer(pTextRenderer),
      m_pSubsystemProvider(pSubsystemProvider)
{
    physics::RigidBodySubsystemSettings rigidBodySubsystemSettings;
    rigidBodySubsystemSettings.MaxRigidBodies = 5;
    m_pRigidBodySubsystem = 
        pAllocator->Create<physics::RigidBodySubsystem>(rigidBodySubsystemSettings);

    physics::CollisionSubsystemSettings collisionSubsystemSettings;
    m_pCollisionSubsystem = pAllocator->Create<sputter::physics::CollisionSubsystem>(collisionSubsystemSettings);

    render::MeshSubsystemSettings meshSubsystemSettings;
    meshSubsystemSettings.MaxVertexCount = 20;
    meshSubsystemSettings.MaxMeshCount = 20;
    m_pMeshSubsystem = pAllocator->Create<sputter::render::MeshSubsystem>(meshSubsystemSettings);

    m_pSubsystemProvider->AddSubsystem(m_pRigidBodySubsystem);
    m_pSubsystemProvider->AddSubsystem(m_pCollisionSubsystem);
    m_pSubsystemProvider->AddSubsystem(m_pMeshSubsystem);
}

void GameInstance::Initialize()
{
    m_pGameState->CountdownTimerHandle = game::TimerSystem::kInvalidTimerHandle;
    m_pGameState->Arena.Initialize(gameconstants::ArenaDimensions);        
    m_pGameState->Camera.SetTranslation(gameconstants::InitialCameraPosition);
    m_pGameState->Player1Score = 0;
    m_pGameState->Player2Score = 0;
    m_pGameState->WinningPlayer = 0;

    SetGameState(GameState::State::Starting);
}

void GameInstance::Restart()
{
    SetGameState(GameState::State::Restarting);
}

void GameInstance::Exit()
{
    SetGameState(GameState::State::Exiting);
}

void GameInstance::Tick(math::FixedPoint dt)
{

}

void GameInstance::Draw()
{
    const glm::mat4 viewMatrix = m_pCamera->ViewMatrix4d();
    m_pMeshSubsystem->Draw(*m_pOrthoMatrix, viewMatrix);
    m_pTextRenderer->SetMatrices(*m_pOrthoMatrix, viewMatrix);
    m_pTextRenderer->SetDepth(-1.f);

    DrawScore(gameconstants::P1ScorePositionX, gameconstants::ScorePositionY, m_pTextRenderer, m_pGameState->Player1Score);
    DrawScore(gameconstants::P2ScorePositionX, gameconstants::ScorePositionY, m_pTextRenderer, m_pGameState->Player2Score);

    const GameState::State CurrentState = m_pGameState->CurrentState;
    if (CurrentState == GameState::State::Paused)
    {
        m_pTextRenderer->DrawText(-160, -20, 5, "PAUSE");
    } 
    else if (CurrentState == GameState::State::Ended/* && !m_pModalPopup*/)
    {
        // CreateEndOfGameModalPopup();
        // m_pModalPopup->SetText(m_pGameState->WinningPlayer == 1 ? "P1 WINS" : "P2 WINS");
    }

}

void GameInstance::SetGameStateChangedCallback(GameStateChangedCallback fnOnGameStateChanged)
{

}

void GameInstance::SetGameState(GameState::State newState)
{
    if (newState == m_pGameState->CurrentState)
    {
        // Nothing to do
        return;
    }

    m_pGameState->CurrentState = newState;

    if (m_fnOngameStateChanged)
    {
        m_fnOngameStateChanged(newState);
    }
    
}