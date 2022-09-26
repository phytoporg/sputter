#include "gamescene.h"
#include "gameconstants.h"
#include "gamestate.h"
#include "paddlearena.h"

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

#include <sputter/ui/modalpopup.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

using namespace sputter;

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
    PaddleArena* pPaddleArena,
    sputter::game::TimerSystem* pTimerSystem,
    sputter::render::Camera* pCamera,
    glm::mat4* pOrthoMatrix,
    sputter::render::VolumetricTextRenderer* pTextRenderer,
    sputter::assets::AssetStorage* pAssetStorage,
    sputter::assets::AssetStorageProvider* pStorageProvider)
    : m_reservedRegion(0x100000),
      m_fixedAllocator("GameState", m_reservedRegion.GetRegionBase(), m_reservedRegion.GetRegionSize()),
      m_pAssetStorageProvider(pStorageProvider),
      m_pTimerSystem(pTimerSystem),
      m_pCamera(pCamera),
      m_pOrthoMatrix(pOrthoMatrix),
      m_pTextRenderer(pTextRenderer),
      m_pWindow(pWindow),
      m_pPaddleArena(pPaddleArena)
{
    physics::RigidBodySubsystemSettings rigidBodySubsystemSettings;
    rigidBodySubsystemSettings.MaxRigidBodies = 5;
    m_pRigidBodySubsystem = 
        m_fixedAllocator.Create<physics::RigidBodySubsystem>(rigidBodySubsystemSettings);

    physics::CollisionSubsystemSettings collisionSubsystemSettings;
    m_pCollisionSubsystem = m_fixedAllocator.Create<sputter::physics::CollisionSubsystem>(collisionSubsystemSettings);

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
          { static_cast<uint32_t>(GLFW_KEY_UP), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_UP) },
          { static_cast<uint32_t>(GLFW_KEY_S), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_DOWN) },
          { static_cast<uint32_t>(GLFW_KEY_DOWN), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_DOWN) },
          { static_cast<uint32_t>(GLFW_KEY_A), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_LEFT) },
          { static_cast<uint32_t>(GLFW_KEY_LEFT), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_LEFT) },
          { static_cast<uint32_t>(GLFW_KEY_D), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_RIGHT) },
          { static_cast<uint32_t>(GLFW_KEY_RIGHT), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_RIGHT) },
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

    m_pGameState = m_fixedAllocator.Create<GameState>(m_pAssetStorageProvider, &m_subsystemProvider);

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

    m_uiTheme.FocusedBorderColor = render::Color::White;
    m_uiTheme.ButtonBorderSize = gameconstants::MainMenuButtonBorderSize;
    m_uiTheme.UnfocusedBorderColor = render::Color::Gray;
    m_uiTheme.ButtonDownAndDisabledBorderColor = render::Color::Red;
    m_uiTheme.ModalBorderSize = 4;
    m_uiTheme.ModalBackgroundColor = render::Color::Black;
}

GameScene::~GameScene()
{
    delete m_pMeshSubsystem;
    m_pMeshSubsystem = nullptr;

    delete m_pInputSubsystem;
    m_pInputSubsystem = nullptr;

    delete m_pScreen;
    m_pScreen = nullptr;
}

void GameScene::Initialize() 
{
    m_pGameState->CurrentState = GameState::State::Starting;
    m_pGameState->Arena.Initialize(gameconstants::ArenaDimensions);        
    m_pGameState->Camera.SetTranslation(gameconstants::InitialCameraPosition);
    m_pGameState->Player1Score = 0;
    m_pGameState->Player2Score = 0;
    m_pGameState->WinningPlayer = 0;

    if (!m_pScreen)
    {
        m_pScreen = new ui::Screen(m_pWindow);

        const sputter::math::Vector2i ModalPosition(-200, -200);
        const sputter::math::Vector2i ModalDimensions(400, 400);
        const sputter::math::Vector2i ModalButtonDimensions(120, 50);
        const char* ppButtonTextEntries[] = { "RESTART", "DONE" };
        m_pModalPopup = new sputter::ui::ModalPopup(
            m_pScreen, &m_uiTheme, m_pTextRenderer,
            ModalPosition, ModalDimensions, ModalButtonDimensions,
            (const char**)ppButtonTextEntries, 2);

        using namespace sputter::ui;
        m_pModalPopup->SetModalPopupOptionSelectedCallback([this](ModalPopup::ModalPopupSelection selection){
            m_pModalPopup->SetVisibility(false);
            if (selection == ModalPopup::ModalPopupSelection::Selection_0)
            {
                // Restart: TODO
            }
            else
            {
                m_pPaddleArena->PreviousSceneFromGame();
            }
        });
        m_pModalPopup->SetVisibility(false);
    }

    m_pScreen->Initialize();
}

void GameScene::Uninitialize() 
{
    if (m_pScreen)
    {
        m_pScreen->Uninitialize();
    }
}

void GameScene::Tick(sputter::math::FixedPoint dt) 
{
    TickFrame(dt);
    PostTickFrame(dt);
}

void GameScene::Draw()
{
    const glm::mat4 viewMatrix = m_pGameState->Camera.ViewMatrix4d();
    m_pMeshSubsystem->Draw(*m_pOrthoMatrix, viewMatrix);
    m_pTextRenderer->SetMatrices(*m_pOrthoMatrix, viewMatrix);
    m_pTextRenderer->SetDepth(-1.f);

    DrawScore(gameconstants::P1ScorePositionX, gameconstants::ScorePositionY, m_pTextRenderer, m_pGameState->Player1Score);
    DrawScore(gameconstants::P2ScorePositionX, gameconstants::ScorePositionY, m_pTextRenderer, m_pGameState->Player2Score);

    if (m_pGameState->CurrentState == GameState::State::Ended)
    {
        m_pModalPopup->SetText(m_pGameState->WinningPlayer == 1 ? "P1 WINS" : "P2 WINS");
        m_pModalPopup->SetVisibility(true);
    }

    m_pScreen->Draw();
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

    m_pScreen->Tick((float)dt);
}

void GameScene::PostTickFrame(math::FixedPoint dt)
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
        math::FPVector3D ballServePosition;
        math::FPVector2D ballServeDirection;
        const math::FixedPoint BallX = m_pGameState->TheBall.GetPosition().GetX();
        if (BallX < math::FPZero)
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

        if (m_pGameState->Player1Score >= gameconstants::ScoreToWin && 
           (m_pGameState->Player1Score - m_pGameState->Player2Score) >= gameconstants::WinningScoreAdvantage)
        {
            m_pGameState->WinningPlayer = 1;
            m_pGameState->CurrentState = GameState::State::Ended;
        }
        else if (m_pGameState->Player2Score >= gameconstants::ScoreToWin && 
                (m_pGameState->Player2Score - m_pGameState->Player1Score) >= gameconstants::WinningScoreAdvantage)
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