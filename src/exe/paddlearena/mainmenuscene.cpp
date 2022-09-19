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
    m_pTimerSystem->CreateFrameTimer(120, OnStartTimerExpire, this);

    m_pScreen = new ui::Screen(m_pWindow);
    m_pVersusAiButton = new ui::Button(m_pScreen);
    m_pVersusAiButton->SetPosition(-100, -100);
    m_pVersusAiButton->SetDimensions(200, 50);
    m_pVersusAiButton->SetBorderSize(3);
    m_pVersusAiButton->SetBorderColor(render::Color::White);
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
    m_pVolumeTextRenderer->DrawText(-380, 100, 5, "PADDLEARENA");
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