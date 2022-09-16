#include "gamescene.h"
#include "gameconstants.h"
#include "gamestate.h"

#include <sputter/assets/assetstorageprovider.h>

#include <sputter/physics/rigidbodysubsystem.h>
#include <sputter/physics/collisionsubsystem.h>

#include <sputter/memory/fixedmemoryallocator.h>

#include <sputter/render/window.h>
#include <sputter/render/meshsubsystem.h>
#include <sputter/render/shaderstorage.h>
#include <sputter/render/fontstorage.h>
#include <sputter/render/volumetrictext.h>

#include <sputter/input/inputsubsystem.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace sputter;

// TODO: Migrate most of this to a game instance class
enum class PaddleArenaInput {
    // Starting out with forreal pong
    INPUT_MOVE_UP,
    INPUT_MOVE_DOWN,
    INPUT_MOVE_LEFT,
    INPUT_MOVE_RIGHT
};

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

GameScene::GameScene(
    render::Window* pWindow,
    sputter::game::TimerSystem* pTimerSystem,
    sputter::assets::AssetStorage* pAssetStorage,
    sputter::assets::AssetStorageProvider* pStorageProvider,
    memory::FixedMemoryAllocator& allocator)
    : m_pAssetStorageProvider(pStorageProvider),
      m_pTimerSystem(pTimerSystem)
{
    physics::RigidBodySubsystemSettings rigidBodySubsystemSettings;
    rigidBodySubsystemSettings.MaxRigidBodies = 5;
    m_pRigidBodySubsystem = 
        allocator.Create<physics::RigidBodySubsystem>(rigidBodySubsystemSettings);

    physics::CollisionSubsystemSettings collisionSubsystemSettings;
    m_pCollisionSubsystem = allocator.Create<sputter::physics::CollisionSubsystem>(collisionSubsystemSettings);

    render::MeshSubsystemSettings meshSubsystemSettings;
    meshSubsystemSettings.MaxVertexCount = 20;
    meshSubsystemSettings.MaxMeshCount = 20;
    m_pMeshSubsystem = new sputter::render::MeshSubsystem(
        meshSubsystemSettings
    );

    sputter::input::InputSubsystemSettings inputSubsystemSettings;
    inputSubsystemSettings.pWindow = pWindow;
    inputSubsystemSettings.PlayerDevices[0] = sputter::input::DeviceType::KeyboardInputDevice;
    inputSubsystemSettings.PlayerDevices[1] = sputter::input::DeviceType::Invalid;

    const std::vector<sputter::input::InputMapEntry> p1InputMap = 
        { 
          { static_cast<uint32_t>(GLFW_KEY_W), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_UP) },
          { static_cast<uint32_t>(GLFW_KEY_S), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_DOWN) },
          { static_cast<uint32_t>(GLFW_KEY_A), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_LEFT) },
          { static_cast<uint32_t>(GLFW_KEY_D), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_RIGHT) },
          };
    inputSubsystemSettings.pInputMapEntryArrays[0] = p1InputMap.data();
    inputSubsystemSettings.pInputMapEntryArrays[1] = nullptr;
        
    inputSubsystemSettings.pNumInputMapEntries[0] = p1InputMap.size();
    inputSubsystemSettings.pNumInputMapEntries[1] = 0;
    m_pInputSubsystem = new sputter::input::InputSubsystem(inputSubsystemSettings);

    m_subsystemProvider.AddSubsystem(m_pRigidBodySubsystem);
    m_subsystemProvider.AddSubsystem(m_pCollisionSubsystem);
    m_subsystemProvider.AddSubsystem(m_pMeshSubsystem);
    m_subsystemProvider.AddSubsystem(m_pInputSubsystem);

    m_pGameState = allocator.Create<GameState>(m_pAssetStorageProvider, &m_subsystemProvider);

    auto pShaderStorage = m_pAssetStorageProvider->GetStorageByType<render::ShaderStorage>();
    if (!pShaderStorage)
    {
        system::LogAndFail("Failed to retrieve shader storage");
    }

    auto pFontStorage = m_pAssetStorageProvider->GetStorageByType<render::FontStorage>();
    if (!pFontStorage)
    {
        system::LogAndFail("Failed to retrieve font storage");
    }
    m_pTextRenderer = new render::VolumetricTextRenderer(m_pAssetStorage, pShaderStorage, pFontStorage);
}

void GameScene::Initialize() 
{
    m_pGameState->CurrentState = GameState::State::Starting;
    m_pGameState->Arena.Initialize(gameconstants::ArenaDimensions);        
    m_pGameState->Camera.Translate(gameconstants::InitialCameraPosition);
}

void GameScene::Uninitialize() 
{
}

void GameScene::Tick(sputter::math::FixedPoint dt) 
{
    TickFrame(dt);
    PostTickFrame(dt);
}

void GameScene::Draw()
{
    static const glm::mat4 OrthoMatrix =
       glm::ortho(
           0.0f, 
           gameconstants::OrthoWidth,
           gameconstants::OrthoHeight,
           0.0f,
           0.0f, 1000.0f);

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

void GameScene::TickFrame(math::FixedPoint dt)
{
    m_pInputSubsystem->Tick(dt);
    m_pRigidBodySubsystem->Tick(dt);

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
        m_pGameState->TheBall.Tick(dt);
        m_pGameState->Arena.Tick(dt);
        m_pGameState->Player1Paddle.Tick(dt);
        m_pGameState->Player2Paddle.Tick(dt);
    }
}

void GameScene::PostTickFrame(math::FixedPoint dt)
{ 
    m_pCollisionSubsystem->PostTick(dt);

    m_pGameState->TheBall.PostTick(dt);
    m_pGameState->Player1Paddle.PostTick(dt);
    m_pGameState->Player2Paddle.PostTick(dt);

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

void GameScene::OnCountdownTimerExpired(game::TimerSystem* pTimerSystem, game::TimerSystem::TimerHandle handle, void* pUserData)
{
    auto pGameScene = reinterpret_cast<GameScene*>(pUserData);

    pGameScene->m_pGameState->CountdownTimeRemaining--;
    if (pGameScene->m_pGameState->CountdownTimeRemaining == 0)
    {
        if (!pTimerSystem->ClearTimer(handle))
        {
            LOG(WARNING) << "Could not clear countdown timer handle\n";
        }
    }
}