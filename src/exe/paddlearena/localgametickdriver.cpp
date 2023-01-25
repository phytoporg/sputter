#include "localgametickdriver.h"
#include "gameinstance.h"

#include <sputter/input/inputsubsystem.h>
#include <sputter/memory/fixedmemoryallocator.h>

using namespace sputter;
using namespace sputter::game;
using namespace sputter::input;

LocalGameTickDriver::LocalGameTickDriver(
    sputter::memory::FixedMemoryAllocator& fixedAllocator,
    sputter::input::InputSubsystem* pInputSubsystem,
    GameInstance* pGameInstance
) : m_pInputSubsystem(pInputSubsystem),
    m_pGameInstance(pGameInstance),
    m_serializer(fixedAllocator)
{}

void LocalGameTickDriver::SetEnableSyncTest(bool enableSyncTest)
{
    m_syncTestEnabled = enableSyncTest;
}

void LocalGameTickDriver::Tick(math::FixedPoint dt)
{
    uint32_t syncTestFrame = m_pGameInstance->GetFrame();
    uint32_t synctestChecksum = 0;
    if (m_syncTestEnabled)
    {
        m_serializer.SaveFrame(syncTestFrame);
        TickOneFrame(dt);
        synctestChecksum = m_serializer.GetChecksum(syncTestFrame);
        m_serializer.LoadFrame(syncTestFrame);
    }

    TickOneFrame(dt);

    if (m_syncTestEnabled)
    {
        if (m_serializer.GetChecksum(syncTestFrame) != synctestChecksum)
        {
            // TODO: Log a thing
        }
    }
}

void LocalGameTickDriver::TickFrames(uint32_t start, uint32_t end, math::FixedPoint dt)
{}

void LocalGameTickDriver::TickOneFrame(math::FixedPoint dt)
{
    m_pInputSubsystem->Tick(dt);

    m_pGameInstance->Tick(dt);
    m_pGameInstance->PostTick(dt);
}