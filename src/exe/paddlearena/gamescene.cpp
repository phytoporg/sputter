#include "gamescene.h"
#include "gameconstants.h"
#include "gamestate.h"
#include "gameinstance.h"
#include "paddlearena.h"

#include <sputter/assets/assetstorageprovider.h>

#include <sputter/render/window.h>
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

    sputter::input::InputMapEntry InputMapping(int32_t from, PaddleArenaInput to)
    {
        return sputter::input::InputMapEntry { 
            static_cast<uint32_t>(from),
            static_cast<uint32_t>(to) 
        }; 
    }
}

GameScene::GameScene(
    render::Window* pWindow,
    PaddleArena* pPaddleArena,
    sputter::game::TimerSystem* pTimerSystem,
    sputter::render::Camera* pCamera,
    glm::mat4* pOrthoMatrix,
    sputter::render::VolumetricTextRenderer* pTextRenderer,
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
    sputter::input::InputSubsystemSettings inputSubsystemSettings;
    inputSubsystemSettings.pWindow = pWindow;
    inputSubsystemSettings.PlayerDevices[0] = sputter::input::DeviceType::KeyboardInputDevice;
    inputSubsystemSettings.PlayerDevices[1] = sputter::input::DeviceType::None;

    const std::vector<sputter::input::InputMapEntry> p1InputMap = { 
        InputMapping(GLFW_KEY_W, PaddleArenaInput::INPUT_MOVE_UP),
        InputMapping(GLFW_KEY_UP, PaddleArenaInput::INPUT_MOVE_UP),
        InputMapping(GLFW_KEY_S, PaddleArenaInput::INPUT_MOVE_DOWN),
        InputMapping(GLFW_KEY_DOWN, PaddleArenaInput::INPUT_MOVE_DOWN),
        InputMapping(GLFW_KEY_A, PaddleArenaInput::INPUT_MOVE_LEFT),
        InputMapping(GLFW_KEY_LEFT, PaddleArenaInput::INPUT_MOVE_LEFT),
        InputMapping(GLFW_KEY_D, PaddleArenaInput::INPUT_MOVE_RIGHT),
        InputMapping(GLFW_KEY_RIGHT, PaddleArenaInput::INPUT_MOVE_RIGHT),
        InputMapping(GLFW_KEY_P, PaddleArenaInput::INPUT_PAUSE),
        InputMapping(GLFW_KEY_SPACE, PaddleArenaInput::INPUT_SERVE),
        InputMapping(GLFW_KEY_J, PaddleArenaInput::INPUT_DASH),
    };
    inputSubsystemSettings.pInputMapEntryArrays[0] = p1InputMap.data();
    inputSubsystemSettings.pInputMapEntryArrays[1] = nullptr;
        
    inputSubsystemSettings.pNumInputMapEntries[0] = p1InputMap.size();
    inputSubsystemSettings.pNumInputMapEntries[1] = 0;
    m_pInputSubsystem = new sputter::input::InputSubsystem(inputSubsystemSettings);

    m_pInputSources[0] = m_pInputSubsystem->GetInputSource(0);
    m_pInputSources[1] = m_pInputSubsystem->GetInputSource(1);

    m_subsystemProvider.AddSubsystem(m_pInputSubsystem);

    GameState* pGameState =
        m_fixedAllocator.Create<GameState>(m_pAssetStorageProvider, &m_subsystemProvider);
    m_pGameInstance = 
        m_fixedAllocator.Create<GameInstance>(
            &m_fixedAllocator,
            &m_subsystemProvider,
            pGameState,
            pTimerSystem,
            pCamera,
            pOrthoMatrix,
            pTextRenderer,
            m_pInputSources[0],
            m_pInputSources[1]);

    m_uiTheme.FocusedBorderColor = render::Color::WHITE;
    m_uiTheme.ButtonBorderSize = gameconstants::MainMenuButtonBorderSize;
    m_uiTheme.UnfocusedBorderColor = render::Color::GRAY;
    m_uiTheme.ButtonDownAndDisabledBorderColor = render::Color::RED;
    m_uiTheme.ModalBorderSize = 4;
    m_uiTheme.ModalBackgroundColor = render::Color::BLACK;
}

GameScene::~GameScene()
{
    delete m_pInputSubsystem;
    m_pInputSubsystem = nullptr;

    delete m_pScreen;
    m_pScreen = nullptr;
}

void GameScene::Initialize() 
{
    if (!m_pScreen)
    {
        m_pScreen = new ui::Screen(m_pWindow);
    }

    m_pScreen->Initialize();
}

void GameScene::Uninitialize() 
{
    if (m_pScreen)
    {
        m_pScreen->Uninitialize();
    }
    DestroyEndOfGameModalPopup();
}

void GameScene::CreateEndOfGameModalPopup()
{
    if (m_pModalPopup)
    {
        LOG(WARNING) << "Modal popup already exists";
        return;
    }

    const char* ppButtonTextEntries[] = {
        gameconstants::GameEndRestartButtonText,
        gameconstants::GameEndExitButtonText
    };
    const uint32_t NumModalButtons = sizeof(ppButtonTextEntries) / sizeof(ppButtonTextEntries[0]);
    m_pModalPopup = new sputter::ui::ModalPopup(
        m_pScreen, &m_uiTheme, m_pTextRenderer,
        gameconstants::GameEndModalPosition,
        gameconstants::GameEndModalDimensions,
        gameconstants::GameEndModalButtonDimensions,
        (const char**)ppButtonTextEntries, NumModalButtons);

    using namespace sputter::ui;
    m_pModalPopup->SetModalPopupOptionSelectedCallback([this](ModalPopup::ModalPopupSelection selection){
        m_pModalPopup->SetVisibility(false);

        if (selection == ModalPopup::ModalPopupSelection::Selection_0)
        {
            m_pGameInstance->Restart();
        }
        else
        {
            m_pGameInstance->Exit();
        }
    });
}

void GameScene::DestroyEndOfGameModalPopup()
{
    if (m_pModalPopup)
    {
        m_pScreen->RemoveChild(m_pModalPopup);
        delete m_pModalPopup;
        m_pModalPopup = nullptr;
    }
}

void GameScene::Tick(sputter::math::FixedPoint dt) 
{
    TickFrame(dt);
    PostTickFrame(dt);
}

void GameScene::Draw()
{
    m_pGameInstance->Draw();
    m_pScreen->Draw();
}

void GameScene::TickFrame(math::FixedPoint dt)
{
    m_pInputSubsystem->Tick(dt);

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

            m_pGameState->CurrentState = GameState::State::Playing;
        }
    }

    if (CurrentState == GameState::State::Playing)
    {
        m_pGameState->TheBall.Tick(dt);
        m_pGameState->Arena.Tick(dt);
        m_pGameState->Player1Paddle.Tick(dt);
        m_pGameState->Player2Paddle.Tick(dt);

        if (CheckPauseInput())
        {
            m_pGameState->CurrentState = GameState::State::Paused;
        }
    }

    if (CurrentState == GameState::State::Paused && CheckPauseInput())
    {
        m_pGameState->CurrentState = GameState::State::Playing;
    }

    if (CurrentState == GameState::State::Restarting)
    {
        DestroyEndOfGameModalPopup();
        Initialize();
    }

    if (CurrentState == GameState::State::Exiting)
    {
        DestroyEndOfGameModalPopup();
        m_pPaddleArena->PreviousSceneFromGame();
    }

    m_pScreen->Tick((float)dt);
}

void GameScene::PostTickFrame(math::FixedPoint dt)
{ 
    const GameState::State CurrentState = m_pGameState->CurrentState;
    if (CurrentState == GameState::State::Playing)
    {
        // m_pCollisionSubsystem->PostTick(dt);

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
            pServingPaddle->AttachBall(&m_pGameState->TheBall);
        }
    }
}

bool GameScene::CheckPauseInput() const
{
    const uint32_t PauseCode = static_cast<uint32_t>(PaddleArenaInput::INPUT_PAUSE);
    return ((m_pInputSources[0] && m_pInputSources[0]->IsInputReleased(PauseCode)) ||
            (m_pInputSources[1] && m_pInputSources[1]->IsInputReleased(PauseCode)));
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
