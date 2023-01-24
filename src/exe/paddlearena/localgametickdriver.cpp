#include "localgametickdriver.h"

using namespace sputter;
using namespace sputter::game;
using namespace sputter::input;

LocalGameTickDriver::LocalGameTickDriver(
    sputter::input::InputSubsystem* pInputSubsystem,
    GameInstance* pGameInstance
) : m_pInputSubsystem(pInputSubsystem),
    m_pGameInstance(pGameInstance)
{}

void LocalGameTickDriver::Tick(math::FixedPoint dt)
{

}

void LocalGameTickDriver::TickFrames(uint32_t start, uint32_t end, math::FixedPoint dt)
{
}

void LocalGameTickDriver::TickOneFrame(math::FixedPoint dt)
{
}