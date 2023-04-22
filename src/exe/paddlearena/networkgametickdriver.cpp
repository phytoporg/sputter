#include "networkgametickdriver.h"
#include "gameinstance.h"
#include "protocol.h"

#include <sputter/memory/fixedmemoryallocator.h>
#include <sputter/input/inputdevice.h>
#include <cstring>

using namespace sputter;
using namespace sputter::input;

// TODO: get packets, manage inputs, etc.

struct NetworkGameTickDriver::InputStorage
{
    InputStorage(IInputDevice* p1InputDevice, IInputDevice* p2InputDevice)
    {
        memset(PlayerInputs, 0, sizeof(PlayerInputs));
        memset(PlayerInputDevices, 0, sizeof(PlayerInputDevices));

        const DeviceType P1DeviceType = p1InputDevice->GetDeviceType();
        const DeviceType P2DeviceType = p2InputDevice->GetDeviceType();

        RELEASE_CHECK(
            (P1DeviceType == DeviceType::Remote && P2DeviceType != P1DeviceType) ||
            (P2DeviceType == DeviceType::Remote && P1DeviceType != P2DeviceType),
            "One player must be local and one player must be remote");

        LocalPlayerIndex = p1InputDevice->GetDeviceType() == DeviceType::Remote ? 1 : 0;

        PlayerInputDevices[0] = p1InputDevice;
        PlayerInputDevices[1] = p2InputDevice;
    }

    uint32_t SampleLocalPlayerDevice() const
    {
        RELEASE_CHECK(LocalPlayerIndex < 2, "Player index out of bound sampling player device");
        return PlayerInputDevices[LocalPlayerIndex] ?
               PlayerInputDevices[LocalPlayerIndex]->SampleGameInputState() :
               0;
    }

    static constexpr int kMaxPlayerInputDevices = 8;
    static constexpr int kMaxPlayerInputFrames = 8;
    IInputDevice* PlayerInputDevices[kMaxPlayerInputDevices];
    uint32_t PlayerInputs[kMaxPlayerInputDevices][kMaxPlayerInputFrames];
    bool FramesConfirmed[kMaxPlayerInputFrames] = {};
    uint8_t LocalPlayerIndex = 0;

    uint8_t Tail = 0;

    // Last confirmed frame entry
    uint8_t Head = 0;
    uint32_t LastConfirmedFrame = 0;
};

NetworkGameTickDriver::NetworkGameTickDriver(
    memory::FixedMemoryAllocator& fixedAllocator,
    InputSubsystem *pInputSubsystem,
    sputter::net::ReliableUDPSession* pReliableUDPSession,
    GameInstance *pGameInstance)
    : m_pInputSubsystem(pInputSubsystem),
      m_pGameInstance(pGameInstance),
      m_pReliableUDPSession(pReliableUDPSession),
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

    const size_t DelayBufferSize = sizeof(uint32_t) * m_inputDelay;
    m_pInputDelayBuffer =
        fixedAllocator.ReserveNext<uint32_t>(DelayBufferSize);
    memset(m_pInputDelayBuffer, 0, DelayBufferSize);
    RELEASE_CHECK(m_pInputDelayBuffer, "Failed to allocate input delay buffer");

    const size_t SendMessageSize = InputsMessage::GetExpectedSize(kNumInputsToSend);
    m_pSendMessage =
        fixedAllocator.ReserveNext<InputsMessage>(SendMessageSize);
    RELEASE_CHECK(m_pSendMessage, "Failed to allocate message send buffer");
    memset(m_pSendMessage, 0, SendMessageSize);
}

void NetworkGameTickDriver::Initialize()
{
    m_serializer.Reset();
}

void NetworkGameTickDriver::Tick(sputter::math::FixedPoint dt)
{
    const uint32_t CurrentFrame = m_pGameInstance->GetFrame();
    const uint32_t LocalPlayerIndex = m_pInputStorage->LocalPlayerIndex;

    // Sample local input and send "delayframes" into the future
    const uint32_t TailIndex = m_pInputStorage->Tail % InputStorage::kMaxPlayerInputFrames;
    const uint32_t LocalInputs = m_pInputStorage->SampleLocalPlayerDevice();
    m_pInputStorage->PlayerInputs[LocalPlayerIndex][TailIndex] = LocalInputs;

    if (!SendNextInputMessage())
    {
        RELEASE_LOGLINE_ERROR(LOG_GAME, "Failed to send inputs to peer");
        // TODO: something more. Are we disconnected?
        return;
    }

    if (CurrentFrame < m_inputDelay)
    {
        // No inputs yet! Won't receive any for these frames either.
        TickOneFrame(dt, 0, 0);
        m_pInputStorage->FramesConfirmed[CurrentFrame] = true;
        m_serializer.SaveFrame(CurrentFrame);
        return;
    }
}

void NetworkGameTickDriver::TickOneFrame(
        sputter::math::FixedPoint dt,
        uint32_t p1InputMask,
        uint32_t p2InputMask)
{
    // TODO
}

bool NetworkGameTickDriver::ReadNextRemotePlayerMessage(
    InputsMessage *pInputMessage,
    size_t messageLen)
{
    if (m_pReliableUDPSession)
    {
        RELEASE_LOGLINE_ERROR(LOG_GAME, "No valid remote session in network game tick driver");
        return false;
    }

    const ssize_t ReadSize =
        m_pReliableUDPSession->TryReadData(
            reinterpret_cast<char *>(&pInputMessage),
            messageLen);
    if (!ReadSize) { return false; }
    if (ReadSize != messageLen || pInputMessage->Header.MessageSize != messageLen)
    {
        RELEASE_LOGLINE_WARNING(LOG_GAME, "Read unexpected number of bytes for inputs message");
        return false;
    }

    return true;
}

bool NetworkGameTickDriver::SendNextInputMessage() const
{
    const size_t SendMessageSize = InputsMessage::GetExpectedSize(kNumInputsToSend);
    m_pSendMessage->Header.Type = MessageType::Inputs;
    m_pSendMessage->Header.MessageSize = SendMessageSize;

    m_pSendMessage->NumFrames = std::min(kNumInputsToSend, m_pGameInstance->GetFrame() - m_pInputStorage->LastConfirmedFrame);
    return false;
}
