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
        RemotePlayerIndex = LocalPlayerIndex ^ 1;

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

    uint32_t GetInputMaskForFrame(uint8_t playerIndex, uint32_t frame, bool predict = false)
    {
        // Provide a predicted result if we're ahead of the last confirmed frame
        if (predict)
        {
            return PlayerInputs[playerIndex][LastConfirmedFrame & kMaxPlayerInputFrames];
        }

        return PlayerInputs[playerIndex][frame % kMaxPlayerInputFrames];
    }

    void SetInputMaskForFrame(uint8_t playerIndex, uint32_t frame, uint32_t inputMask)
    {
        PlayerInputs[playerIndex][frame % kMaxPlayerInputFrames] = inputMask;
    }

    static constexpr int kMaxPlayerInputDevices = 8;
    static constexpr int kMaxPlayerInputFrames = 8;
    IInputDevice* PlayerInputDevices[kMaxPlayerInputDevices];
    uint32_t PlayerInputs[kMaxPlayerInputDevices][kMaxPlayerInputFrames];
    uint8_t LocalPlayerIndex = 0;
    uint8_t RemotePlayerIndex = 0;

    // Last confirmed frame entry
    uint8_t Head = 0;
    int32_t LastConfirmedFrame = -1;
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

    m_pInputSubsystem->SetFrame(CurrentFrame);
    m_pInputSubsystem->Tick(dt);

    // Sample local input and send "delayframes" into the future
    const uint32_t LocalInputs = m_pInputStorage->SampleLocalPlayerDevice();
    m_pInputStorage->SetInputMaskForFrame(LocalPlayerIndex, CurrentFrame + m_inputDelay, LocalInputs);

    if (!SendNextInputMessage())
    {
        RELEASE_LOGLINE_ERROR(LOG_GAME, "Failed to send inputs to peer");
        // TODO: something more. Are we disconnected?
        return;
    }

    bool receivedRemotePlayerMessage = true;
    if (!ReadNextRemotePlayerMessage(m_pSendMessage))
    {
        RELEASE_LOGLINE_VERBOSE(LOG_GAME, "Did not read any remote input");
        // TODO: something more. Are we disconnected?
        receivedRemotePlayerMessage = false;
    }

    if (CurrentFrame < m_inputDelay)
    {
        // No inputs yet! Won't receive any for these frames either.
        TickOneFrame(dt, 0, 0);
        m_serializer.SaveFrame(CurrentFrame);
        m_pInputStorage->LastConfirmedFrame = CurrentFrame;
        return;
    }

    if (!receivedRemotePlayerMessage)
    {
        TickOneFrame(
            dt,
            m_pInputStorage->GetInputMaskForFrame(0, CurrentFrame, 0 != LocalPlayerIndex),
            m_pInputStorage->GetInputMaskForFrame(1, CurrentFrame, 1 != LocalPlayerIndex));
        return;
    }

    const NetworkGameTickDriver::RollbackTickInfo RollbackTickInfo =
        ProcessRemoteInputsMessage(m_pSendMessage);

    // Shouldn't StartFrame always be the confirmed frame?
    RELEASE_CHECK(
        RollbackTickInfo.StartFrame >= 0,
        "Unexpected rollback start frame");
    RELEASE_CHECK(
        RollbackTickInfo.StartFrame == m_pInputStorage->LastConfirmedFrame + 1,
        "Rollback not starting with last confirmed frame");
    if (RollbackTickInfo.StartFrame < CurrentFrame)
    {
        m_serializer.LoadFrame(m_pInputStorage->LastConfirmedFrame);
    }

    const uint32_t TargetFrame =
        std::min(CurrentFrame, static_cast<uint32_t>(RollbackTickInfo.TargetFrame));
    RELEASE_LOGLINE_INFO(
        LOG_GAME,
        "Beginning rollback: %d -> %d",
        RollbackTickInfo.StartFrame,
        TargetFrame);
    for (uint32_t frame = RollbackTickInfo.StartFrame; frame <= CurrentFrame; ++frame)
    {
        const bool PredictP1Input = 0 != LocalPlayerIndex && frame > TargetFrame;
        const bool PredictP2Input = 1 != LocalPlayerIndex && frame > TargetFrame;
        m_pInputSubsystem->SetFrame(frame);
        TickOneFrame(
                dt,
                m_pInputStorage->GetInputMaskForFrame(0, frame, PredictP1Input),
                m_pInputStorage->GetInputMaskForFrame(1, frame, PredictP2Input));
        if (frame <= TargetFrame)
        {
            // TODO: evaluate and compare checksums
            m_pInputStorage->LastConfirmedFrame = frame;
            if (frame == TargetFrame)
            {
                m_serializer.SaveFrame(m_pInputStorage->LastConfirmedFrame);
            }
        }
    }
}

void NetworkGameTickDriver::TickOneFrame(
        sputter::math::FixedPoint dt,
        uint32_t p1InputMask,
        uint32_t p2InputMask)
{
    m_pGameInstance->Tick(dt, p1InputMask, p2InputMask);
    m_pGameInstance->PostTick(dt);
}

bool NetworkGameTickDriver::ReadNextRemotePlayerMessage(InputsMessage *pInputMessage)
{
    if (!m_pReliableUDPSession)
    {
        RELEASE_LOGLINE_ERROR(LOG_GAME, "No valid remote session in network game tick driver");
        return false;
    }

    const size_t MaxMessageLen = InputsMessage::GetExpectedSize(kNumInputsToSend);
    const ssize_t ReadSize =
        m_pReliableUDPSession->TryReadData(
            reinterpret_cast<char *>(pInputMessage),
            MaxMessageLen);
    if (!ReadSize) { return false; }
    if (pInputMessage->Header.MessageSize != InputsMessage::GetExpectedSize(pInputMessage->NumFrames))
    {
        RELEASE_LOGLINE_WARNING(LOG_GAME, "Read unexpected number of bytes for inputs message");
        return false;
    }

    return true;
}

bool NetworkGameTickDriver::SendNextInputMessage() const
{
    m_pSendMessage->Header.Type = MessageType::Inputs;

    const uint32_t CurrentFrame = m_pGameInstance->GetFrame() + m_inputDelay;
    if (m_pInputStorage->LastConfirmedFrame < 0)
    {
        m_pSendMessage->NumFrames = std::min(kNumInputsToSend, CurrentFrame + 1);
    }
    else
    {
        m_pSendMessage->NumFrames =
            std::min(kNumInputsToSend, CurrentFrame - m_pInputStorage->LastConfirmedFrame);
    }
    RELEASE_CHECK(m_pSendMessage->NumFrames <= kNumInputsToSend, "Sending too many frames");

    m_pSendMessage->StartFrame = CurrentFrame - m_pSendMessage->NumFrames + 1;
    RELEASE_LOGLINE_VERBOSE(
        LOG_GAME,
        "Sending - START: %u - NUM: %hhu",
        m_pSendMessage->StartFrame,
        m_pSendMessage->NumFrames);
    const uint8_t LocalPlayerIndex = m_pInputStorage->LocalPlayerIndex;
    for (uint32_t i = 0; i < m_pSendMessage->NumFrames; ++i)
    {
        m_pSendMessage->GameInputMasks[i] =
            m_pInputStorage->GetInputMaskForFrame(LocalPlayerIndex, m_pSendMessage->StartFrame + i);

        if (m_pSendMessage->GameInputMasks[i] != 0)
        {
            RELEASE_LOGLINE_INFO(
                LOG_GAME,
                "INPUT TO SEND: Frame %u, Mask %u",
                m_pSendMessage->StartFrame + i,
                m_pSendMessage->GameInputMasks[i]);
        }
    }
    const size_t SendMessageSize = InputsMessage::GetExpectedSize(kNumInputsToSend);
    m_pSendMessage->Header.MessageSize = InputsMessage::GetExpectedSize(m_pSendMessage->NumFrames);

    const size_t SentBytes = m_pReliableUDPSession->EnqueueSendData(
        reinterpret_cast<char*>(m_pSendMessage), SendMessageSize);
    if (SentBytes != SendMessageSize)
    {
        RELEASE_LOGLINE_ERROR(LOG_GAME, "Failed to inputs message");
        return false;
    }

    return true;
}

NetworkGameTickDriver::RollbackTickInfo
NetworkGameTickDriver::ProcessRemoteInputsMessage(InputsMessage *pInputMessage)
{
    NetworkGameTickDriver::RollbackTickInfo rollbackTickInfo
    {
        .StartFrame = -1,
        .TargetFrame = -1
    };
    RELEASE_LOGLINE_INFO(LOG_GAME, "ProcessMessage++");

    for (uint32_t i = 0; i < pInputMessage->NumFrames; ++i)
    {
        const int32_t Frame = pInputMessage->StartFrame + i;
        if (Frame <= m_pInputStorage->LastConfirmedFrame)
        {
            RELEASE_LOGLINE_INFO(
                LOG_GAME,
                "Skipping frame which predates last confirmed frame: %d <= %d, mask = %u",
                Frame,
                m_pInputStorage->LastConfirmedFrame,
                pInputMessage->GameInputMasks[i]);
            continue;
        }
        else if (rollbackTickInfo.StartFrame < 0)
        {
            RELEASE_LOGLINE_INFO(
                LOG_GAME,
                "Initializing start frame to %u, mask = %u",
                Frame,
                pInputMessage->GameInputMasks[i]);
            rollbackTickInfo.StartFrame = Frame;
        }

        rollbackTickInfo.TargetFrame = Frame;

        const uint32_t FrameRemoteInputMask = pInputMessage->GameInputMasks[i];
        if (FrameRemoteInputMask != 0)
        {
            //REMOVEME
            RELEASE_LOGLINE_INFO(
                LOG_GAME,
                "Received nonzero remote input mask: %u",
                FrameRemoteInputMask);
            //REMOVEME
        }
        m_pInputStorage->SetInputMaskForFrame(
            m_pInputStorage->RemotePlayerIndex, Frame, FrameRemoteInputMask);
    }

    RELEASE_LOGLINE_INFO(
            LOG_GAME,
            "START = %d, TARGET = %d",
            rollbackTickInfo.StartFrame, rollbackTickInfo.TargetFrame);

    RELEASE_LOGLINE_INFO(LOG_GAME, "ProcessMessage--");
    return rollbackTickInfo;
}
