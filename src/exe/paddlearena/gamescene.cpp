#include "gamescene.h"
#include "gameconstants.h"
#include "gamestate.h"
#include "gameinstance.h"
#include "paddlearena.h"

#include <sputter/assets/assetstorageprovider.h>

#include <sputter/log/log.h>

#include <sputter/render/window.h>
#include <sputter/render/volumetrictext.h>

#include <sputter/input/inputsubsystem.h>

#include <sputter/ui/modalpopup.h>

#include <sputter/net/protocol.h>

#include <GLFW/glfw3.h>

#include <memory>

using namespace sputter;

namespace {
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
    sputter::render::Camera* pCamera,
    glm::mat4* pOrthoMatrix,
    sputter::render::VolumetricTextRenderer* pTextRenderer,
    sputter::assets::AssetStorageProvider* pStorageProvider)
    : m_reservedRegion(0x100000),
      m_fixedAllocator("GameState", m_reservedRegion.GetRegionBase(), m_reservedRegion.GetRegionSize()),
      m_pAssetStorageProvider(pStorageProvider),
      m_pTextRenderer(pTextRenderer),
      m_pWindow(pWindow),
      m_pPaddleArena(pPaddleArena),
      m_pCamera(pCamera),
      m_pOrthoMatrix(pOrthoMatrix)
{
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

    // Always start with no modal
    delete m_pModalPopup;
    m_pModalPopup = nullptr;
}

void GameScene::Initialize() 
{
    if (!CreateInputSubsystem())
    {
        RELEASE_LOGLINE_ERROR(LOG_GAME, "Failed to create input subsystem");
        return;
    }

    if (!m_pGameInstance)
    {
        m_pGameInstance =
            m_fixedAllocator.Create<GameInstance>(
                &m_fixedAllocator,
                m_pAssetStorageProvider,
                &m_subsystemProvider,
                &m_timerSystem,
                m_pCamera,
                m_pOrthoMatrix,
                m_pTextRenderer,
                m_pInputSources[0],
                m_pInputSources[1]);
}

    if (IsLocalGame() && !m_pGameTickDriver)
    {
        m_pGameTickDriver =
            m_fixedAllocator.Create<LocalGameTickDriver>(
                m_fixedAllocator,
                m_pInputSubsystem,
                m_pGameInstance);
    }
    else if (!IsLocalGame() || !m_pGameTickDriver)
    {
        m_pGameTickDriver =
            m_fixedAllocator.Create<NetworkGameTickDriver>(
                m_fixedAllocator,
                m_pInputSubsystem,
                m_pPaddleArena->GetUDPSession(),
                m_pGameInstance);
        m_waitingForGameStart = true;
    }

    if (!m_pScreen)
    {
        m_pScreen = new ui::Screen(m_pWindow);
    }

    m_pScreen->Initialize();
    m_pGameInstance->Initialize();
    m_pGameTickDriver->Initialize();

    m_pGameInstance->SetGameStateChangedCallback([this](GameState::State newState)
    {
        if (newState == GameState::State::Starting)
        {
            m_enableTickDriverTick = true;
        }
        else if (newState == GameState::State::Ended)
        {
            CreateEndOfGameModalPopup();
            m_enableTickDriverTick = false;
        }
        else if (newState == GameState::State::Exiting)
        {
            DestroyEndOfGameModalPopup();
            m_pPaddleArena->PreviousSceneFromGame();
        }
        else if (newState == GameState::State::Restarting)
        {
            DestroyEndOfGameModalPopup();
            Initialize();
        }
    });
    m_enableTickDriverTick = true;
    m_waitingForRestartReady = false;
    m_sentClientReady = false;
    m_waitingForGameStart = false;
    m_sentRestartReady = false;
}

bool GameScene::CreateInputSubsystem()
{
    if (m_pInputSubsystem)
    {
        // Nothing to do
        return true;
    }

    input::InputSubsystemSettings inputSubsystemSettings;
    inputSubsystemSettings.pWindow = m_pWindow;

    const std::vector<input::InputMapEntry> p1InputMap = {
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

    if (IsLocalGame())
    {
        inputSubsystemSettings.PlayerDevices[0] = input::DeviceType::KeyboardInputDevice;
        inputSubsystemSettings.PlayerDevices[1] = input::DeviceType::None;

        inputSubsystemSettings.pInputMapEntryArrays[0] = p1InputMap.data();
        inputSubsystemSettings.pInputMapEntryArrays[1] = nullptr;

        inputSubsystemSettings.pNumInputMapEntries[0] = p1InputMap.size();
        inputSubsystemSettings.pNumInputMapEntries[1] = 0;
    }
    else
    {
        const GameMode GameMode = m_pPaddleArena->GetGameMode();

        // TODO: Assign player slots some other way
        uint32_t localIndex;
        uint32_t remoteIndex;
        if (GameMode == GameMode::Client)
        {
            localIndex = 1;
            remoteIndex = 0;

        }
        else if (GameMode == GameMode::Server)
        {
            localIndex = 0;
            remoteIndex = 1;
        }
        else
        {
            system::LogAndFail("Unexpected game mode!");
        }

        inputSubsystemSettings.PlayerDevices[remoteIndex] = input::DeviceType::Remote;
        inputSubsystemSettings.PlayerDevices[localIndex] = input::DeviceType::KeyboardInputDevice;
        inputSubsystemSettings.pReliableUDPSession = m_pPaddleArena->GetUDPSession();

        inputSubsystemSettings.pInputMapEntryArrays[localIndex] = p1InputMap.data();
        inputSubsystemSettings.pInputMapEntryArrays[remoteIndex] = nullptr;

        inputSubsystemSettings.pNumInputMapEntries[localIndex] = p1InputMap.size();
        inputSubsystemSettings.pNumInputMapEntries[remoteIndex] = 0;
    }

    m_pInputSubsystem = new input::InputSubsystem(inputSubsystemSettings);

    m_pInputSources[0] = m_pInputSubsystem->GetInputSource(0);
    m_pInputSources[1] = m_pInputSubsystem->GetInputSource(1);

    m_subsystemProvider.AddSubsystem(m_pInputSubsystem);

    return true;
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
        RELEASE_LOGLINE_WARNING(LOG_GAME, "Modal popup already exists");
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
    m_pModalPopup->SetModalPopupOptionSelectedCallback(
        [this](ModalPopup::ModalPopupSelection selection){
        m_pModalPopup->SetVisibility(false);

        if (selection == ModalPopup::ModalPopupSelection::Selection_0)
        {
            if (IsLocalGame())
            {
                HandleRoundResetAsLocalPlayer();
            }
            else
            {
                HandleRoundResetNonLocal();
            }
        }
        else
        {
            m_pGameInstance->Exit();
        }
    });

    if (!IsLocalGame())
    {
        m_waitingForRestartReady = true;
        m_sentRestartReady = false;
    }

    int16_t p1Score;
    int16_t p2Score;
    m_pGameInstance->GetCurrentScore(&p1Score, &p2Score);
    m_pModalPopup->SetText(
        p1Score > p2Score ? "P1 WINS" : "P2 WINS");
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
    if (m_enableTickDriverTick)
    {
        m_pGameTickDriver->Tick(dt);
    }

    m_pScreen->Tick((float)dt);

    if (!m_sentClientReady)
    {
        sputter::net::ProtocolPtr spProtocol = m_pPaddleArena->GetProtocol();
        if (spProtocol->SendClientReadyMessage(m_pPaddleArena->GetClientId()))
        {
            m_sentClientReady = true;
            m_waitingForGameStart = true;
        }
    }
    if (m_waitingForGameStart)
    {
        RELEASE_CHECK(false, "NOT READY YET");
        // if (spProtocol->ReceiveGameStartMessage())
        // {
        // }
    } else if (m_waitingForRestartReady)
    {
        // TODO
    }
    else if (!m_waitingForRestartReady && m_sentRestartReady)
    {
        m_pGameInstance->Restart();
    }
}

void GameScene::Draw()
{
    m_pGameInstance->Draw();
    m_pScreen->Draw();
}

void GameScene::HandleRoundResetAsLocalPlayer()
{
    m_pGameInstance->Restart();
}

void GameScene::HandleRoundResetNonLocal()
{
    net::ReliableUDPSession* pSession = m_pPaddleArena->GetUDPSession();
    if (!pSession)
    {
        RELEASE_LOGLINE_ERROR(LOG_GAME, "Attempted to reset, but UDP session is null");
        m_pGameInstance->Exit();
        return;
    }

    static const RestartReadyMessage s_RestartReady;
    const size_t BytesSent = pSession->SendReliable(
            reinterpret_cast<const char *>(&s_RestartReady),
            sizeof(s_RestartReady));
    if (BytesSent != sizeof(s_RestartReady))
    {
        RELEASE_LOGLINE_ERROR(LOG_GAME, "Failed to send restart ready message");
        m_pGameInstance->Exit();
        return;
    }

    pSession->Flush();
    m_sentRestartReady = true;
    RELEASE_LOGLINE_INFO(LOG_GAME, "Sent restart ready message");
}

bool GameScene::IsLocalGame() const
{
    return m_pPaddleArena->GetGameMode() == GameMode::Local;
}
