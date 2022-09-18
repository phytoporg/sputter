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
    }
}

class PaddleArena;

class MainMenuScene : public sputter::game::IScene
{
public:
    MainMenuScene(
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

    sputter::render::VolumetricTextRenderer*    m_pVolumeTextRenderer = nullptr;
    sputter::game::TimerSystem*                 m_pTimerSystem = nullptr;
    sputter::game::TimerSystem::TimerHandle     m_startTimerHandle = sputter::game::TimerSystem::kInvalidTimerHandle;

    sputter::render::Camera*                    m_pCamera = nullptr;
    glm::mat4*                                  m_pOrthoMatrix = nullptr;

    PaddleArena*                                m_pPaddleArena = nullptr;
};