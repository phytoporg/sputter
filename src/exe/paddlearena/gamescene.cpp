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

    m_pGameInstance = 
        m_fixedAllocator.Create<GameInstance>(
            &m_fixedAllocator,
            m_pAssetStorageProvider,
            &m_subsystemProvider,
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

    // Always start with no modal
    delete m_pModalPopup;
    m_pModalPopup = nullptr;
}

void GameScene::Initialize() 
{
    if (!m_pScreen)
    {
        m_pScreen = new ui::Screen(m_pWindow);
    }

    m_pScreen->Initialize();
    m_pGameInstance->Initialize();

    m_pGameInstance->SetGameStateChangedCallback([this](GameState::State newState)
    {
        if (newState == GameState::State::Ended)
        {
            CreateEndOfGameModalPopup();
        }
        else if (newState == GameState::State::Exiting)
        {
            CloseEndOfGameModalPopup();
            m_pPaddleArena->PreviousSceneFromGame();
        }
        else if (newState == GameState::State::Restarting)
        {
            CloseEndOfGameModalPopup();
            Initialize();
        }
    });
}

void GameScene::Uninitialize() 
{
    if (m_pScreen)
    {
        m_pScreen->Uninitialize();
    }
    CloseEndOfGameModalPopup();
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

    int16_t p1Score;
    int16_t p2Score;
    m_pGameInstance->GetCurrentScore(&p1Score, &p2Score);
    m_pModalPopup->SetText(
        p1Score > p2Score ? "P1 WINS" : "P2 WINS");
}

void GameScene::CloseEndOfGameModalPopup()
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
    m_pGameInstance->Tick(dt);
    m_pScreen->Tick((float)dt);
}

void GameScene::PostTickFrame(math::FixedPoint dt)
{ 
    m_pGameInstance->PostTick(dt);
}