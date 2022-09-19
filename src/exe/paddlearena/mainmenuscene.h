#pragma once

#include <sputter/render/camera.h>

#include <sputter/game/scene.h>
#include <sputter/game/timersystem.h>

#include <glm/glm.hpp>

// Forward declarations
namespace sputter
{
    namespace render
    {
        class Camera;
        class VolumetricTextRenderer;
        class Window;
    }

    namespace ui
    {
        class Screen;
        class Button;
    }
}

class PaddleArena;

class MainMenuScene : public sputter::game::IScene
{
public:
    MainMenuScene(
       sputter::render::Window* pWindow,
       PaddleArena* pPaddleArena,
       sputter::game::TimerSystem* pTimerSystem,
       sputter::render::VolumetricTextRenderer* pVolumeTextRenderer,
       sputter::render::Camera* pCamera,
       glm::mat4* pOrthoMatrix);

    virtual void Initialize() override;
    virtual void Uninitialize() override;

    virtual void Tick(sputter::math::FixedPoint dt) override;
    virtual void Draw() override;

private:
    static void OnStartTimerExpire(
        sputter::game::TimerSystem* pTimerSystem,
        sputter::game::TimerSystem::TimerHandle timerHandle,
        void* pUserData);

    sputter::render::Window*                    m_pWindow = nullptr;
    sputter::render::VolumetricTextRenderer*    m_pVolumeTextRenderer = nullptr;
    sputter::game::TimerSystem*                 m_pTimerSystem = nullptr;
    sputter::game::TimerSystem::TimerHandle     m_startTimerHandle = sputter::game::TimerSystem::kInvalidTimerHandle;

    sputter::render::Camera*                    m_pCamera = nullptr;
    glm::mat4*                                  m_pOrthoMatrix = nullptr;

    sputter::ui::Screen*                        m_pScreen = nullptr;
    sputter::ui::Button*                        m_pVersusAiButton = nullptr;

    PaddleArena*                                m_pPaddleArena = nullptr;
};