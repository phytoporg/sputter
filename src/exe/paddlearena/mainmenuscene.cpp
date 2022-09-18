#include "mainmenuscene.h"
#include "gameconstants.h"
#include "paddlearena.h"

#include <sputter/render/volumetrictext.h>
#include <sputter/render/drawshapes.h>

#include <sputter/game/timersystem.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace sputter;

MainMenuScene::MainMenuScene(
    PaddleArena* pPaddleArena,
    sputter::game::TimerSystem* pTimerSystem,
    sputter::render::VolumetricTextRenderer* pVolumeTextRenderer,
    sputter::render::Camera* pCamera,
    glm::mat4* pOrthoMatrix)
    : m_pVolumeTextRenderer(pVolumeTextRenderer),
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

    const sputter::render::Color Red{255, 0, 0};
    sputter::render::shapes::DrawRect(100, 100, 50, 50, 3, Red);
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