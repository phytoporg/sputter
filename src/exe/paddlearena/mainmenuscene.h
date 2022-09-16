#pragma once

#include <sputter/render/camera.h>

#include <sputter/game/scene.h>
#include <sputter/game/timersystem.h>

// Forward declarations
namespace sputter
{
    namespace render
    {
        class VolumetricTextRenderer;
    }
}

class PaddleArena;

class MainMenuScene : public sputter::game::IScene
{
public:
    MainMenuScene(
       PaddleArena* pPaddleArena,
       sputter::render::VolumetricTextRenderer* pVolumeTextRenderer,
       sputter::game::TimerSystem* pTimerSystem);

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

    sputter::render::Camera                     m_mainMenuCamera;
    PaddleArena*                                m_pPaddleArena = nullptr;
};