#include "localgametickdriver.h"
#include "gameinstance.h"

#include <sputter/input/inputsubsystem.h>
#include <sputter/memory/fixedmemoryallocator.h>
#include <sputter/system/system.h>

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
{
    m_serializer.RegisterSerializableObject(m_pGameInstance);
}

void LocalGameTickDriver::SetEnableSyncTest(bool enableSyncTest)
{
    m_syncTestEnabled = enableSyncTest;
}

void LocalGameTickDriver::Tick(math::FixedPoint dt)
{
    const uint32_t InitialFrame = m_pGameInstance->GetFrame();
    m_pInputSubsystem->SetFrame(InitialFrame);
    m_pInputSubsystem->Tick(dt);

    uint32_t synctestChecksum = 0;
    if (m_syncTestEnabled)
    {
        m_serializer.SaveFrame(InitialFrame);

        TickOneFrame(dt);

        const uint32_t CurrentFrame = m_pGameInstance->GetFrame();

        m_serializer.SaveFrame(CurrentFrame);
        synctestChecksum = m_serializer.GetChecksum(CurrentFrame);
        m_serializer.LoadFrame(InitialFrame);
    }

    TickOneFrame(dt);

    if (m_syncTestEnabled)
    {
        const uint32_t CurrentFrame = m_pGameInstance->GetFrame();
        m_serializer.SaveFrame(CurrentFrame);
        if (m_serializer.GetChecksum(CurrentFrame) != synctestChecksum)
        {
            sputter::system::LogAndFail("Checksum mismatch!");
        }
    }
}

void LocalGameTickDriver::TickOneFrame(math::FixedPoint dt)
{
    m_pGameInstance->Tick(dt);
    m_pGameInstance->PostTick(dt);
}