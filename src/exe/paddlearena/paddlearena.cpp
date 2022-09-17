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
#include <sputter/math/fixedpoint.h>

using namespace sputter;

PaddleArena::PaddleArena(
        render::Window* pWindow,
        const std::string& assetStoragePath,
        memory::FixedMemoryAllocator allocator) 
    : m_assetStorage(assetStoragePath),
      m_pWindow(pWindow),
      m_storageProvider(&m_assetStorage)
{
    m_storageProvider.AddResourceStorageByType(&m_shaderStorage);
    m_storageProvider.AddResourceStorageByType(&m_fontStorage);

    m_pTextRenderer = new sputter::render::VolumetricTextRenderer(&m_assetStorage, &m_shaderStorage, &m_fontStorage);

    m_pWindow->SetDimensionConstraints(gameconstants::MinWidth, gameconstants::MinHeight, -1, -1);

    // Set up window resizing handler
    sputter::render::Window::WindowResizeCallback onResize = [this](sputter::render::Window* pWindow, uint32_t width, uint32_t height)
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

    // Set up the scene stack
    m_pMainMenuScene = new MainMenuScene(this, m_pTextRenderer, &m_timerSystem);
    m_pGameScene = new GameScene(pWindow, &m_timerSystem, m_pTextRenderer, &m_assetStorage, &m_storageProvider, allocator);
    game::IScene* ppScenes[] = { m_pMainMenuScene, m_pGameScene };
    m_pSceneStack = new game::SceneStack(ppScenes, sizeof(ppScenes) / sizeof(ppScenes[0]));
}

PaddleArena::~PaddleArena() 
{
    delete m_pSceneStack;
    m_pSceneStack = nullptr;

    if (m_pGameScene)
    {
        m_pGameScene->Uninitialize();

        delete m_pGameScene;
        m_pSceneStack = nullptr;
    }

    if (m_pGameScene)
    {
        m_pGameScene->Uninitialize();
        delete m_pGameScene;
        m_pGameScene = nullptr;
    }

    delete m_pTextRenderer;
}

void PaddleArena::Tick(math::FixedPoint dt)
{
    m_timerSystem.Tick(/* TODO: dt */);
    m_pSceneStack->Tick(dt);
}

void PaddleArena::Draw()
{
    m_pSceneStack->Draw();
}

bool PaddleArena::StartGame()
{
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