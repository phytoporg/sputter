#pragma once

#include <sputter/game/scene.h>

class MainMenuScene : public sputter::game::IScene
{
public:
    MainMenuScene(); // WhAt GOES IN Heree??

    virtual void Initialize() override;
    virtual void Uninitialize() override;

    virtual void Tick(sputter::math::FixedPoint dt) override;
    virtual void Draw() override;

private:
};