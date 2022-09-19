// Not sure where windows.h is entering the picture and I don't have time to dig through /showIncludes
// walls of text atm. This does the trick for now.
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define NOMINMAX
#endif

#include "paddlearena.h"
#include "gameconstants.h"
#include "mainmenuscene.h"
#include "gamescene.h"

#include <sputter/render/window.h>
#include <sputter/render/volumetrictext.h>
#include <sputter/render/drawshapes.h>
#include <sputter/math/fixedpoint.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace sputter;

PaddleArena::PaddleArena(
        render::Window* pWindow,
        const std::string& assetStoragePath) 
    : m_assetStorage(assetStoragePath),
      m_pWindow(pWindow),
      m_storageProvider(&m_assetStorage)
{
    m_storageProvider.AddResourceStorageByType(&m_shaderStorage);
    m_storageProvider.AddResourceStorageByType(&m_fontStorage);

    m_pTextRenderer = new render::VolumetricTextRenderer(&m_assetStorage, &m_shaderStorage, &m_fontStorage);

    m_pWindow->SetDimensionConstraints(gameconstants::MinWidth, gameconstants::MinHeight, -1, -1);

    // Set up window resizing handler
    render::Window::WindowResizeCallback onResize = [this](sputter::render::Window* pWindow, uint32_t width, uint32_t height)
    {
        const float WindowAspectRatio = static_cast<float>(height) / width;

        // In cases where the window aspect ratio is smaller than the target aspect ratio, constrain the width so that
        // the viewport height won't exceed the actual window height.
        float viewportWidth = width;
        if (WindowAspectRatio < gameconstants::TargetAspectRatio)
        {
            viewportWidth = height / gameconstants::TargetAspectRatio;
        }

        float viewportHeight = viewportWidth * gameconstants::TargetAspectRatio;
        
        // Keep the viewport centered within the window
        const float dWidth = width - viewportWidth;
        const float ViewportX = dWidth > 0.f ? dWidth / 2.f : 0.f;

        const float dHeight = height - viewportHeight;
        const float ViewportY = dHeight > 0.f ? dHeight / 2.f : 0.f;

        glViewport(ViewportX, ViewportY, viewportWidth, viewportHeight);
    };

    // Contrive an initial "resize" event before setting the callback
    onResize(m_pWindow, m_pWindow->GetWidth(), m_pWindow->GetHeight());
    m_pWindow->SetWindowResizeCallback(onResize);
}

PaddleArena::~PaddleArena() 
{
    delete m_pSceneStack;
    m_pSceneStack = nullptr;

    if (m_pGameScene)
    {
        m_pGameScene->Uninitialize();

        delete m_pGameScene;
        m_pGameScene = nullptr;
    }

    if (m_pMainMenuScene)
    {
        m_pMainMenuScene->Uninitialize();
        delete m_pMainMenuScene;
        m_pMainMenuScene = nullptr;
    }

    delete m_pTextRenderer;

    render::shapes::UninitializeLineRenderer();
}

void PaddleArena::Tick(math::FixedPoint dt)
{
    m_timerSystem.Tick(/* TODO: dt */);
    m_pSceneStack->Tick(dt);
}

void PaddleArena::Draw()
{
    m_pSceneStack->Draw();
    render::shapes::FlushLineRenderer();
}

bool PaddleArena::StartGame()
{
    m_camera.SetTranslation(gameconstants::InitialCameraPosition);
    m_orthoMatrix = glm::ortho(
           0.0f, 
           gameconstants::OrthoWidth,
           gameconstants::OrthoHeight,
           0.0f,
           0.0f, 1000.0f);

    // TODO: Initialize UI
    render::shapes::InitializeLineRenderer(&m_assetStorage, &m_shaderStorage, m_camera.ViewMatrix4d(), m_orthoMatrix);

    // Set up the scene stack
    m_pMainMenuScene = new MainMenuScene(m_pWindow, this, &m_timerSystem, m_pTextRenderer, &m_camera, &m_orthoMatrix);
    m_pGameScene = new GameScene(m_pWindow, &m_timerSystem, &m_camera, &m_orthoMatrix, m_pTextRenderer, &m_assetStorage, &m_storageProvider);
    game::IScene* ppScenes[] = { m_pMainMenuScene, m_pGameScene };
    m_pSceneStack = new game::SceneStack(ppScenes, sizeof(ppScenes) / sizeof(ppScenes[0]));
    m_pSceneStack->Initialize();

    return true;
}

void PaddleArena::NextSceneFromMainMenu()
{
    if (m_pSceneStack->GetCurrentScene() != m_pMainMenuScene)
    {
        system::LogAndFail("Unexpected scene calling main menu event");
    }
    
    m_pSceneStack->PushToNextScene();
}