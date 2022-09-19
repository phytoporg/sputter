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
{}

void MainMenuScene::Initialize() 
{
    m_pCamera->SetTranslation(gameconstants::InitialCameraPosition);
    const glm::mat4 viewMatrix = m_pCamera->ViewMatrix4d();
    m_pVolumeTextRenderer->SetMatrices(*m_pOrthoMatrix, viewMatrix);

    // TODO: Remove this timeout; transition should happen on button press.
    m_pTimerSystem->CreateFrameTimer(120, OnStartTimerExpire, this);

    m_pScreen = new ui::Screen(m_pWindow);
    m_pVersusAiButton = new ui::Button(m_pScreen);
    m_pVersusAiButton->SetPosition(gameconstants::VersusAiButtonPositionX, gameconstants::VersusAiButtonPositionY);
    m_pVersusAiButton->SetDimensions(gameconstants::VersusAiButtonDimensionX, gameconstants::VersusAiButtonDimensionY);
    m_pVersusAiButton->SetBorderSize(gameconstants::MainMenuButtonBorderSize);
    m_pVersusAiButton->SetBorderColor(gameconstants::MainMenuButtonBorderColor);
}

void MainMenuScene::Uninitialize() 
{
    if (m_pScreen)
    {
        m_pScreen->RemoveChild(m_pVersusAiButton);
    }
    
    delete m_pVersusAiButton;
    m_pVersusAiButton = nullptr;

    delete m_pScreen;
    m_pScreen = nullptr;
}

void MainMenuScene::Tick(math::FixedPoint dt) 
{
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

void 
MainMenuScene::OnStartTimerExpire(
    game::TimerSystem* pTimerSystem,
    game::TimerSystem::TimerHandle timerHandle,
    void* pUserData)
{
    auto pScene = static_cast<MainMenuScene*>(pUserData);
    pScene->m_startTimerHandle = game::TimerSystem::kInvalidTimerHandle;
    pScene->m_pPaddleArena->NextSceneFromMainMenu();
}