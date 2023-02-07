#include "gameinstance.h"
#include "gameconstants.h"
#include "paddlearena.h"

#include <sputter/game/timersystem.h>

#include <sputter/core/functorstorage.h>

#include <sputter/log/log.h>

#include <sputter/assets/assetstorageprovider.h>

#include <sputter/input/inputsource.h>
#include <sputter/input/inputsubsystem.h>

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
using namespace sputter::core;

GameInstance::GameInstance(
    memory::FixedMemoryAllocator* pAllocator,
    assets::AssetStorageProvider* pAssetStorageProvider,
    game::SubsystemProvider* pSubsystemProvider,
    game::TimerSystem* pTimerSystem,
    render::Camera* pCamera,
    glm::mat4* pOrthoMatrix,
    render::VolumetricTextRenderer* pTextRenderer, 
    sputter::input::InputSource* pP1InputSource,
    sputter::input::InputSource* pP2InputSource)
    : m_pTimerSystem(pTimerSystem),
      m_pCamera(pCamera),
      m_pOrthoMatrix(pOrthoMatrix),
      m_pTextRenderer(pTextRenderer),
      m_pSubsystemProvider(pSubsystemProvider),
      m_pAssetStorageProvider(pAssetStorageProvider)
{
    physics::RigidBodySubsystemSettings rigidBodySubsystemSettings;
    m_pRigidBodySubsystem = 
        pAllocator->Create<physics::RigidBodySubsystem>(rigidBodySubsystemSettings);

    physics::CollisionSubsystemSettings collisionSubsystemSettings;
    m_pCollisionSubsystem = pAllocator->Create<sputter::physics::CollisionSubsystem>(collisionSubsystemSettings);

    render::MeshSubsystemSettings meshSubsystemSettings;
    m_pMeshSubsystem = pAllocator->Create<sputter::render::MeshSubsystem>(meshSubsystemSettings);

    m_pSubsystemProvider->AddSubsystem(m_pRigidBodySubsystem);
    m_pSubsystemProvider->AddSubsystem(m_pCollisionSubsystem);
    m_pSubsystemProvider->AddSubsystem(m_pMeshSubsystem);
    sputter::game::SubsystemProvider::SetSubsystemProviderAddress(m_pSubsystemProvider);

    m_pInputSources[0] = pP1InputSource;
    m_pInputSources[1] = pP2InputSource;

    m_pGameState = pAllocator->Create<GameState>();
    GameState::SetGameStateAddress(m_pGameState);
}

bool GameInstance::Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes)
{
    if (!m_pTimerSystem->Serialize(pBuffer, pBytesWrittenOut, maxBytes)) { return false; }
    if (!m_pRigidBodySubsystem->Serialize(pBuffer, pBytesWrittenOut, maxBytes)) { return false; }
    if (!m_pCollisionSubsystem->Serialize(pBuffer, pBytesWrittenOut, maxBytes)) { return false; }
    if (!m_pGameState->Serialize(pBuffer, pBytesWrittenOut, maxBytes)) { return false; }

    return true;
}

bool GameInstance::Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes)
{
    if (!m_pTimerSystem->Deserialize(pBuffer, pBytesReadOut, maxBytes)) { return false; }
    if (!m_pRigidBodySubsystem->Deserialize(pBuffer, pBytesReadOut, maxBytes)) { return false; }
    if (!m_pCollisionSubsystem->Deserialize(pBuffer, pBytesReadOut, maxBytes)) { return false; }
    if (!m_pGameState->Deserialize(pBuffer, pBytesReadOut, maxBytes)) { return false; }

    return true;
}

void GameInstance::Initialize()
{
    // Register timer callback functor during initialization
    functorstorage::RegisterFunctor(reinterpret_cast<intptr_t>(&GameInstance::OnCountdownTimerExpired));

    m_pGameState->CountdownTimerHandle = game::TimerSystem::kInvalidTimerHandle;
    m_pGameState->Arena.Initialize(gameconstants::ArenaDimensions);        
    m_pGameState->Camera.SetTranslation(gameconstants::InitialCameraPosition);
    m_pGameState->Player1Score = 0;
    m_pGameState->Player2Score = 0;
    m_pGameState->WinningPlayer = 0;
    m_pGameState->Frame = 0;

    if (m_pInputSources[0]) { m_pInputSources[0]->Reset(); }
    if (m_pInputSources[1]) { m_pInputSources[1]->Reset(); }

    if (!functorstorage::IsFunctorRegistrationLocked())
    {
        functorstorage::LockFunctorRegistration();
    }

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

void GameInstance::Tick(math::FixedPoint dt, uint32_t p1InputState, uint32_t p2InputState)
{
    if (m_pInputSources[0]) { m_pInputSources[0]->SetInputState(p1InputState); }
    if (m_pInputSources[1]) { m_pInputSources[1]->SetInputState(p2InputState); }

    m_pTimerSystem->Tick();

    const GameState::State CurrentState = m_pGameState->CurrentState;
    if (CurrentState == GameState::State::Starting)
    {
        m_pTextRenderer->DrawText(
            gameconstants::GetReadyPositionX,
            gameconstants::GetReadyPositionY,
            gameconstants::GetReadySize,
            gameconstants::GetReadyString);

        // TODO: Need a way to check inputs to advance the state.
        if (m_pGameState->CountdownTimerHandle == game::TimerSystem::kInvalidTimerHandle)
        {
            const int8_t LoopCount = gameconstants::StartCountdownSeconds;
            const uint32_t TimerFrames = 60; // 1sec
            m_pGameState->CountdownTimerHandle = m_pTimerSystem->CreateLoopingFrameTimer(TimerFrames, LoopCount, OnCountdownTimerExpired, this);
            m_pGameState->CountdownTimeRemaining = gameconstants::StartCountdownSeconds;
        }

        const char CountdownChar = '0' + static_cast<char>(m_pGameState->CountdownTimeRemaining);
        char pCountdownString[2] = { CountdownChar, '\0' };

        if (m_pGameState->CountdownTimeRemaining > 0)
        {
            m_pTextRenderer->DrawText(
                gameconstants::StartCountdownPositionX,
                gameconstants::StartCountdownPositionY,
                gameconstants::StartCountdownSize,
                pCountdownString);
        }
        else
        {
            // TODO: Clean up unnecessary state once serving behavior is settled
            m_pGameState->TheBall.Initialize(
                gameconstants::BallDimensions,
                gameconstants::BallStartPosition,
                gameconstants::BallStartDirection);

            using namespace sputter::math;
            m_pGameState->Player1Paddle.Initialize(
                FPVector2D(gameconstants::PaddleWidth, gameconstants::PaddleHeight),
                gameconstants::P1PaddleStartPosition);
            m_pGameState->Player2Paddle.Initialize(
                FPVector2D(gameconstants::PaddleWidth, gameconstants::PaddleHeight),
                gameconstants::P2PaddleStartPosition);

            m_pGameState->Player1Paddle.AttachBall(&m_pGameState->TheBall);
            SetGameState(GameState::State::Playing);
        }
    } 
    else if (CurrentState == GameState::State::Playing) 
    {
        m_pGameState->TheBall.Tick(dt);
        m_pGameState->Arena.Tick(dt);
        m_pGameState->Player1Paddle.Tick(dt);
        m_pGameState->Player2Paddle.Tick(dt);

        if (CheckPauseInput())
        {
            SetGameState(GameState::State::Paused);
        }
    } 
    else if (CurrentState == GameState::State::Paused && CheckPauseInput())
    {
        SetGameState(GameState::State::Playing);
    } 
    else if (CurrentState == GameState::State::Restarting)
    {
        Initialize();
    }
    else if (CurrentState == GameState::State::Exiting)
    {
        // NOOP for now
    }
}

void GameInstance::PostTick(sputter::math::FixedPoint dt)
{
    const GameState::State CurrentState = m_pGameState->CurrentState;
    if (CurrentState == GameState::State::Playing)
    {
        m_pCollisionSubsystem->PostTick(dt);
        m_pGameState->TheBall.PostTick(dt);
        m_pGameState->Player1Paddle.PostTick(dt);
        m_pGameState->Player2Paddle.PostTick(dt);
    }

    // Do we need to reset the ball?
    if (m_pGameState->TheBall.IsDead() &&
        m_pGameState->CurrentState == GameState::State::Playing)
    {
        Paddle* pServingPaddle = nullptr;
        const math::FixedPoint BallX = m_pGameState->TheBall.GetPosition().GetX();
        if (BallX < math::FPZero)
        {
            m_pGameState->Player2Score++;
            pServingPaddle = &m_pGameState->Player1Paddle;
        }
        else
        {
            m_pGameState->Player1Score++;
            pServingPaddle = &m_pGameState->Player2Paddle;
        }

        if (m_pGameState->Player1Score >= gameconstants::ScoreToWin && 
           (m_pGameState->Player1Score - m_pGameState->Player2Score) >= gameconstants::WinningScoreAdvantage)
        {
            m_pGameState->WinningPlayer = 1;
            SetGameState(GameState::State::Ended);
        }
        else if (m_pGameState->Player2Score >= gameconstants::ScoreToWin && 
                (m_pGameState->Player2Score - m_pGameState->Player1Score) >= gameconstants::WinningScoreAdvantage)
        {
            m_pGameState->WinningPlayer = 2;
            SetGameState(GameState::State::Ended);
        }
        else
        {
            // Ideally we wait a few frames. Handle that later!
            pServingPaddle->AttachBall(&m_pGameState->TheBall);
        }
    }

    m_pGameState->Frame++;
}

uint32_t GameInstance::GetFrame() const
{
    return m_pGameState->Frame;
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
}

void GameInstance::GetCurrentScore(int16_t* pP1ScoreOut, int16_t* pP2ScoreOut)
{
    *pP1ScoreOut = m_pGameState->Player1Score;
    *pP2ScoreOut = m_pGameState->Player2Score;
}

void GameInstance::SetGameStateChangedCallback(GameStateChangedCallback fnOnGameStateChanged)
{
    m_fnOngameStateChanged = fnOnGameStateChanged;
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

bool GameInstance::CheckPauseInput() const
{    
    const uint32_t PauseCode = static_cast<uint32_t>(PaddleArenaInput::INPUT_PAUSE);
    return ((m_pInputSources[0] && m_pInputSources[0]->IsInputReleased(PauseCode)) ||
            (m_pInputSources[1] && m_pInputSources[1]->IsInputReleased(PauseCode)));
}

void GameInstance::OnCountdownTimerExpired(void* pUserData)
{
    GameInstance* pGameInstance = static_cast<GameInstance*>(pUserData);

    pGameInstance->m_pGameState->CountdownTimeRemaining--;
    if (pGameInstance->m_pGameState->CountdownTimeRemaining == 0)
    {
        if (!pGameInstance->m_pTimerSystem->ClearTimer(pGameInstance->m_pGameState->CountdownTimerHandle))
        {
            RELEASE_LOG_WARNING_(LOG_GAME, "Could not clearn countdown timer handle\n");
        }
    }
}