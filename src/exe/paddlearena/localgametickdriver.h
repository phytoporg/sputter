#pragma once

#include <sputter/game/tickdriver.h>

namespace sputter { namespace input {
    class InputSubsystem;
}}

class GameInstance;

class LocalGameTickDriver : public sputter::game::ITickDriver
{
public:
    LocalGameTickDriver(
        sputter::input::InputSubsystem* pInputSubsystem,
        GameInstance* pGameInstance);

    virtual void Tick(sputter::math::FixedPoint dt) override;

private:
    virtual void TickFrames(uint32_t start, uint32_t end, sputter::math::FixedPoint dt) override;
    virtual void TickOneFrame(sputter::math::FixedPoint dt) override;

    sputter::input::InputSubsystem* m_pInputSubsystem = nullptr;
    GameInstance*                   m_pGameInstance = nullptr;
};