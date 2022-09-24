#include "mainmenuscene.h"
#include "gameconstants.h"
#include "paddlearena.h"

#include <sputter/render/volumetrictext.h>
#include <sputter/render/drawshapes.h>

#include <sputter/game/timersystem.h>

#include <sputter/ui/screen.h>
#include <sputter/ui/button.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace sputter;

MainMenuScene::MainMenuScene(
    sputter::render::Window* pWindow,
    PaddleArena* pPaddleArena,
    sputter::game::TimerSystem* pTimerSystem,
    sputter::render::VolumetricTextRenderer* pVolumeTextRenderer,
    sputter::render::Camera* pCamera,
    glm::mat4* pOrthoMatrix)
    : m_pWindow(pWindow),
      m_pVolumeTextRenderer(pVolumeTextRenderer),
      m_pTimerSystem(pTimerSystem),
      m_pPaddleArena(pPaddleArena),
      m_pCamera(pCamera),
      m_pOrthoMatrix(pOrthoMatrix)
{
    m_uiTheme.FocusedBorderColor = render::Color::White;
    m_uiTheme.ButtonBorderSize = gameconstants::MainMenuButtonBorderSize;
    m_uiTheme.UnfocusedBorderColor = render::Color::Gray;
    m_uiTheme.ButtonDownAndDisabledBorderColor = render::Color::Red;
}

void MainMenuScene::Initialize() 
{
    m_pCamera->SetTranslation(gameconstants::InitialCameraPosition);
    const glm::mat4 viewMatrix = m_pCamera->ViewMatrix4d();
    m_pVolumeTextRenderer->SetMatrices(*m_pOrthoMatrix, viewMatrix);

    m_pScreen = new ui::Screen(m_pWindow);
    m_pVersusAiButton = new ui::Button(m_pScreen, &m_uiTheme, "VS AI");
    m_pVersusAiButton->SetFontRenderer(m_pVolumeTextRenderer);
    m_pVersusAiButton->SetPosition(
        gameconstants::VersusAiButtonPositionX,
        gameconstants::VersusAiButtonPositionY);
    m_pVersusAiButton->SetDimensions(
        gameconstants::MainMenuButtonDimensionX,
        gameconstants::MainMenuButtonDimensionY);
    m_pVersusAiButton->SetButtonPressedCallback([this](){
        m_pPaddleArena->NextSceneFromMainMenu();
    });

    m_pVersusPlayerButton = new ui::Button(m_pScreen, &m_uiTheme, "VS PLAYER");
    m_pVersusPlayerButton->SetFontRenderer(m_pVolumeTextRenderer);
    m_pVersusPlayerButton->SetPosition(
        gameconstants::VersusAiButtonPositionX,
        gameconstants::VersusAiButtonPositionY -
        gameconstants::MainMenuButtonMarginTop);
    m_pVersusPlayerButton->SetDimensions(
        gameconstants::MainMenuButtonDimensionX,
        gameconstants::MainMenuButtonDimensionY);
    m_pVersusPlayerButton->SetButtonIsDisabled(true);

    {
        using namespace sputter::ui;
        m_pVersusAiButton->SetNavigationLink(m_pVersusPlayerButton, NavigationDirections::Down);
        m_pVersusPlayerButton->SetNavigationLink(m_pVersusAiButton, NavigationDirections::Up);
    }
}

MainMenuScene::~MainMenuScene()
{
    delete m_pVersusAiButton;
    m_pVersusAiButton = nullptr;

    delete m_pVersusPlayerButton;
    m_pVersusPlayerButton = nullptr;

    delete m_pScreen;
    m_pScreen = nullptr;
}

void MainMenuScene::Uninitialize() 
{
    if (m_pScreen)
    {
        m_pScreen->RemoveChild(m_pVersusAiButton);
        m_pScreen->RemoveChild(m_pVersusPlayerButton);
    }
}

void MainMenuScene::Tick(math::FixedPoint dt) 
{
    if (!m_pScreen)
    {
        system::LogAndFail("Screen is unexpectedly null in MainMenuScene");
    }

    m_pScreen->Tick((float)dt);
}

void MainMenuScene::Draw() 
{
    m_pVolumeTextRenderer->DrawText(
        gameconstants::MainMenuGameTitlePositionX,
        gameconstants::MainMenuGameTitlePositionY,
        gameconstants::MainMenuGameTitleSize,
        gameconstants::GameTitleString);
    m_pScreen->Draw();
}