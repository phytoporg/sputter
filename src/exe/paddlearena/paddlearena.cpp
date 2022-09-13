// Not sure where windows.h is entering the picture and I don't have time to dig through /showIncludes
// walls of text atm. This does the trick for now.
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define NOMINMAX
#endif

#include "paddlearena.h"
#include "gamestate.h"
#include "gameconstants.h"

#include <sputter/physics/rigidbodysubsystem.h>

#include <sputter/render/meshsubsystem.h>
#include <sputter/render/shaderstorage.h>
#include <sputter/render/texturestorage.h>
#include <sputter/render/shader.h>

#include <sputter/math/fpconstants.h>
#include <sputter/math/fixedpoint.h>
#include <sputter/math/fpvector2d.h>

#include <sputter/assets/imagedata.h>
#include <sputter/assets/textdata.h>

#include <sputter/input/inputsubsystem.h>

#include <sputter/system/system.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace sputter;

namespace 
{
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

PaddleArena::PaddleArena(
        render::Window* pWindow,
        const std::string& assetStoragePath,
        memory::FixedMemoryAllocator allocator) 
    : m_pGameState(nullptr),
      m_assetStorage(assetStoragePath),
      m_pWindow(pWindow),
      m_storageProvider(&m_assetStorage)
{
    physics::RigidBodySubsystemSettings rigidBodySubsystemSettings;
    rigidBodySubsystemSettings.MaxRigidBodies = 5;
    m_pRigidbodySubsystem = 
        allocator.Create<sputter::physics::RigidBodySubsystem>(rigidBodySubsystemSettings);

    physics::CollisionSubsystemSettings collisionSubsystemSettings;
    m_pCollisionSubsystem = 
        allocator.Create<sputter::physics::CollisionSubsystem>(collisionSubsystemSettings);

    sputter::render::MeshSubsystemSettings meshSubsystemSettings;
    meshSubsystemSettings.MaxVertexCount = 20;
    meshSubsystemSettings.MaxMeshCount = 20;
    m_pMeshSubsystem = new sputter::render::MeshSubsystem(
        meshSubsystemSettings
    );

    sputter::input::InputSubsystemSettings inputSubsystemSettings;
    inputSubsystemSettings.pWindow = m_pWindow;

    inputSubsystemSettings.PlayerDevices[0] = sputter::input::DeviceType::KeyboardInputDevice;
    inputSubsystemSettings.PlayerDevices[1] = sputter::input::DeviceType::Invalid;

    const std::vector<sputter::input::InputMapEntry> p1InputMap = 
        { 
          { static_cast<uint32_t>(GLFW_KEY_W),   static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_UP) },
          { static_cast<uint32_t>(GLFW_KEY_S), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_DOWN) },
          { static_cast<uint32_t>(GLFW_KEY_A), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_LEFT) },
          { static_cast<uint32_t>(GLFW_KEY_D), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_RIGHT) },
          };
    inputSubsystemSettings.pInputMapEntryArrays[0] = p1InputMap.data();
    inputSubsystemSettings.pInputMapEntryArrays[1] = nullptr;
        
    inputSubsystemSettings.pNumInputMapEntries[0] = p1InputMap.size();
    inputSubsystemSettings.pNumInputMapEntries[1] = 0;
    m_pInputSubsystem = new sputter::input::InputSubsystem(inputSubsystemSettings);

    m_subsystemProvider.AddSubsystem(m_pRigidbodySubsystem);
    m_subsystemProvider.AddSubsystem(m_pCollisionSubsystem);
    m_subsystemProvider.AddSubsystem(m_pMeshSubsystem);
    m_subsystemProvider.AddSubsystem(m_pInputSubsystem);

    m_storageProvider.AddResourceStorageByType(&m_shaderStorage);

    m_pGameState = allocator.Create<GameState>(&m_storageProvider, &m_subsystemProvider);

    m_pTextRenderer = new sputter::render::VolumetricTextRenderer(&m_assetStorage, &m_shaderStorage, &m_fontStorage);
}

PaddleArena::~PaddleArena() {}

void PaddleArena::Tick(math::FixedPoint deltaTime)
{
    m_pInputSubsystem->Tick(deltaTime);
    m_pRigidbodySubsystem->Tick(deltaTime);

    const GameState::State CurrentState = m_pGameState->CurrentState;
    if (CurrentState == GameState::State::Starting)
    {
        m_pTextRenderer->DrawText(-380, 10, 5, "PADDLEARENA");

        // TODO: Need a way to check inputs to advance the state.
        if (m_pGameState->CountdownTimerHandle == game::TimerSystem::kInvalidTimerHandle)
        {
            const int8_t LoopCount = 3;
            const uint32_t TimerFrames = 60; // 1sec
            m_pGameState->CountdownTimerHandle = m_timerSystem.CreateLoopingFrameTimer(TimerFrames, LoopCount, OnCountdownTimerExpired, this);
        }

        const char CountdownChar = '0' + static_cast<char>(m_pGameState->CountdownTimeRemaining);
        char pCountdownString[2] = { CountdownChar, '\0' };

        if (m_pGameState->CountdownTimeRemaining > 0)
        {
            m_pTextRenderer->DrawText(-20, -130, 6, pCountdownString);
        }
        else
        {
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

            m_pGameState->CurrentState = GameState::State::Playing;
        }
    }

    if (CurrentState == GameState::State::Playing)
    {
        m_pGameState->TheBall.Tick(deltaTime);
        m_pGameState->Arena.Tick(deltaTime);
        m_pGameState->Player1Paddle.Tick(deltaTime);
        m_pGameState->Player2Paddle.Tick(deltaTime);
    }
    
    m_timerSystem.Tick();
}

void PaddleArena::PostTick(math::FixedPoint deltaTime)
{
    m_pCollisionSubsystem->PostTick(deltaTime);

    m_pGameState->TheBall.PostTick(deltaTime);
    m_pGameState->Player1Paddle.PostTick(deltaTime);
    m_pGameState->Player2Paddle.PostTick(deltaTime);

    // Do we need to reset the ball?
    if (m_pGameState->TheBall.IsDead() &&
        m_pGameState->CurrentState == GameState::State::Playing)
    {
        math::FPVector3D ballServePosition;
        math::FPVector2D ballServeDirection;
        const math::FixedPoint BallY = m_pGameState->TheBall.GetPosition().GetY();
        if (BallY < math::FPZero)
        {
            m_pGameState->Player2Score++;

            ballServePosition = gameconstants::BallServePositionLeft;
            ballServeDirection = gameconstants::BallServeDirectionLeft;
        }
        else
        {
            m_pGameState->Player1Score++;

            ballServePosition = gameconstants::BallServePositionRight;
            ballServeDirection = gameconstants::BallServeDirectionRight;
        }

        if (m_pGameState->Player1Score > gameconstants::ScoreToWin && 
           (m_pGameState->Player1Score - m_pGameState->Player2Score) >= 2)
        {
            m_pGameState->WinningPlayer = 1;
            m_pGameState->CurrentState = GameState::State::Ended;
        }
        else if (m_pGameState->Player2Score > gameconstants::ScoreToWin && 
                (m_pGameState->Player2Score - m_pGameState->Player1Score) >= 2)
        {
            m_pGameState->WinningPlayer = 2;
            m_pGameState->CurrentState = GameState::State::Ended;
        }
        else
        {
            // Ideally we wait a few frames. Handle that later!
            m_pGameState->TheBall.Reset(
                ballServePosition,
                ballServeDirection
                );
        }
    }
}

void PaddleArena::Draw()
{
    static const glm::mat4 OrthoMatrix =
       glm::ortho(
           0.0f, 
           gameconstants::OrthoWidth,
           gameconstants::OrthoHeight,
           0.0f,
           0.0f, 1000.0f);

    // TODO: Put in window init & resize handler, express in terms of desired
    // aspect ratio
    glViewport(0, 0, m_pWindow->GetWidth(), m_pWindow->GetHeight());

    const glm::mat4 viewMatrix = m_pGameState->Camera.ViewMatrix4d();
    m_pMeshSubsystem->Draw(OrthoMatrix, viewMatrix);

    m_pTextRenderer->SetMatrices(OrthoMatrix, viewMatrix);
    DrawScore(gameconstants::P1ScorePositionX, gameconstants::ScorePositionY, m_pTextRenderer, m_pGameState->Player1Score);
    DrawScore(gameconstants::P2ScorePositionX, gameconstants::ScorePositionY, m_pTextRenderer, m_pGameState->Player2Score);

    if (m_pGameState->CurrentState == GameState::State::Ended)
    {
        const std::string WinString = 
            m_pGameState->WinningPlayer == 1 ? "P1 WINS!" : "P2 WINS!";
        m_pTextRenderer->DrawText(
            gameconstants::WinMessagePositionX,
            gameconstants::WinMessagePositionY,
            gameconstants::WinMessageSize,
            WinString.c_str());
    }
}

bool PaddleArena::StartGame()
{
    using namespace sputter::math;

    m_pGameState->CurrentState = GameState::State::Starting;

    m_pGameState->Arena.Initialize(FPVector2D(800, 600));        
    m_pGameState->Camera.Translate(FPVector3D(-500, 400, -200));

    return true;
}

bool PaddleArena::SaveGameState(
    void** pBuffer,
    size_t* pSize,
    size_t* pChecksum,
    int frame)
{
    return false;
}

bool PaddleArena::LoadGameState(void** pBuffer, size_t size)
{
    return false;
}

bool PaddleArena::AdvanceFrame()
{
    return false;
}

void PaddleArena::OnCountdownTimerExpired(sputter::game::TimerSystem* pTimerSystem, sputter::game::TimerSystem::TimerHandle handle, void* pUserData)
{
    auto pArena = reinterpret_cast<PaddleArena*>(pUserData);

    pArena->m_pGameState->CountdownTimeRemaining--;
    if (pArena->m_pGameState->CountdownTimeRemaining == 0)
    {
        if (!pTimerSystem->ClearTimer(handle))
        {
            LOG(WARNING) << "Could not clear countdown timer handle\n";
        }
    }
}