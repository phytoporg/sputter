#include "localgametickdriver.h"
#include "gameinstance.h"

#include <sputter/input/inputsubsystem.h>
#include <sputter/memory/fixedmemoryallocator.h>
#include <sputter/system/system.h>

#include <sputter/input/inputdevice.h>
#include <sputter/input/inputsource.h>

#include <sputter/log/framestatelogger.h>

using namespace sputter;
using namespace sputter::game;
using namespace sputter::input;
using namespace sputter::log;

struct LocalGameTickDriver::InputStorage
{
    InputStorage(IInputDevice* p1InputDevice, IInputDevice* p2InputDevice)
    {
        PlayerInputDevices[0] = p1InputDevice;
        PlayerInputDevices[1] = p2InputDevice;
    }

    uint32_t SamplePlayerDevice(uint8_t playerIndex) const
    {
        RELEASE_CHECK(playerIndex < 2, "Player index out of bound sampling player device");
        return PlayerInputDevices[playerIndex] ? 
                   PlayerInputDevices[playerIndex]->SampleGameInputState() :
                   0;
    }

    static const int kMaxPlayerInputs = 8;
    IInputDevice* PlayerInputDevices[kMaxPlayerInputs] = {};
};

LocalGameTickDriver::LocalGameTickDriver(
    sputter::memory::FixedMemoryAllocator& fixedAllocator,
    sputter::input::InputSubsystem* pInputSubsystem,
    GameInstance* pGameInstance
) : m_pInputSubsystem(pInputSubsystem),
    m_pGameInstance(pGameInstance),
    m_serializer(fixedAllocator)
{
    m_serializer.RegisterSerializableObject(m_pGameInstance);

    InputSource* playerInputSources[2] = {
        m_pInputSubsystem->GetInputSource(0),
        m_pInputSubsystem->GetInputSource(1),
    };
    m_pInputStorage = fixedAllocator.Create<InputStorage>(
        playerInputSources[0] ? playerInputSources[0]->GetInputDevice() : nullptr,
        playerInputSources[1] ? playerInputSources[1]->GetInputDevice() : nullptr
    );
    RELEASE_CHECK(m_pInputStorage, "Failed to allocate input storage in LocalGameTickDriver");
}

void LocalGameTickDriver::Initialize()
{
    m_serializer.Reset();
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

    const uint32_t P1InputMask = m_pInputStorage->SamplePlayerDevice(0);
    const uint32_t P2InputMask = m_pInputStorage->SamplePlayerDevice(1);

    static constexpr int OldFrameLogSlot = 0;
    static constexpr int NewFrameLogSlot = 1;

    uint32_t synctestChecksum = 0;
    if (m_syncTestEnabled)
    {
        m_serializer.SaveFrame(InitialFrame);

        TickOneFrame(dt, P1InputMask, P2InputMask);

        const uint32_t CurrentFrame = m_pGameInstance->GetFrame();

        FrameStateLogger::SetSlot(OldFrameLogSlot);
        m_serializer.SaveFrame(CurrentFrame);
        synctestChecksum = m_serializer.GetChecksum(CurrentFrame);
        m_serializer.LoadFrame(InitialFrame);
    }

    TickOneFrame(dt, P1InputMask, P2InputMask);

    if (m_syncTestEnabled)
    {
        const uint32_t CurrentFrame = m_pGameInstance->GetFrame();

        FrameStateLogger::SetSlot(NewFrameLogSlot);
        m_serializer.SaveFrame(CurrentFrame);
        if (m_serializer.GetChecksum(CurrentFrame) != synctestChecksum)
        {
            FrameStateLogger::LogFrameSlot(
                OldFrameLogSlot,
                CurrentFrame,
                "old");
            FrameStateLogger::LogFrameSlot(
                NewFrameLogSlot,
                CurrentFrame,
                "new");
            sputter::system::LogAndFail("Checksum mismatch!");
        }
    }
}

void LocalGameTickDriver::TickOneFrame(math::FixedPoint dt, uint32_t p1InputMask, uint32_t p2InputMask)
{
    m_pGameInstance->Tick(dt, p1InputMask, p2InputMask);
    m_pGameInstance->PostTick(dt);
}