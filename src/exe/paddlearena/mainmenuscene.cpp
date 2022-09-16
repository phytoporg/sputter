#include "mainmenuscene.h"
#include "gameconstants.h"
#include "paddlearena.h"

#include <sputter/render/volumetrictext.h>

#include <sputter/game/timersystem.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace sputter;

MainMenuScene::MainMenuScene(
    PaddleArena* pPaddleArena,
    render::VolumetricTextRenderer* pVolumeTextRenderer,
    game::TimerSystem* pTimerSystem)
    : m_pVolumeTextRenderer(pVolumeTextRenderer),
      m_pTimerSystem(pTimerSystem),
      m_pPaddleArena(pPaddleArena)
{}

void MainMenuScene::Initialize() 
{
    static const glm::mat4 OrthoMatrix =
       glm::ortho(
           0.0f, 
           gameconstants::OrthoWidth,
           gameconstants::OrthoHeight,
           0.0f,
           0.0f, 1000.0f);
    m_mainMenuCamera.Translate(gameconstants::InitialCameraPosition);
    const glm::mat4 viewMatrix = m_mainMenuCamera.ViewMatrix4d();
    m_pVolumeTextRenderer->SetMatrices(OrthoMatrix, viewMatrix);
    m_pTimerSystem->CreateFrameTimer(60, OnStartTimerExpire, this);
}

void MainMenuScene::Uninitialize() 
{
}

void MainMenuScene::Tick(math::FixedPoint dt) 
{
}

void MainMenuScene::Draw() 
{
    m_pVolumeTextRenderer->DrawText(-280, 0, 5, "MAINMENU");
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