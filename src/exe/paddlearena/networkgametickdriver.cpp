#include "networkgametickdriver.h"
#include "gameinstance.h"

#include <sputter/memory/fixedmemoryallocator.h>
#include <sputter/input/inputdevice.h>
#include <cstring>

using namespace sputter;
using namespace sputter::input;

struct NetworkGameTickDriver::InputStorage
{
    InputStorage(InputSource* pP1InputSource, InputSource* pP2InputSource)
    {
        memset(PlayerInputSources, 0, sizeof(PlayerInputSources));
        memset(PlayerInputDevices, 0, sizeof(PlayerInputDevices));

        IInputDevice* p1InputDevice = pP1InputSource->GetInputDevice();
        IInputDevice* p2InputDevice = pP2InputSource->GetInputDevice();

        const DeviceType P1DeviceType = p1InputDevice->GetDeviceType();
        const DeviceType P2DeviceType = p2InputDevice->GetDeviceType();

        RELEASE_CHECK(
            (P1DeviceType == DeviceType::Remote && P2DeviceType != P1DeviceType) ||
            (P2DeviceType == DeviceType::Remote && P1DeviceType != P2DeviceType),
            "One player must be local and one player must be remote");

        LocalPlayerIndex = p1InputDevice->GetDeviceType() == DeviceType::Remote ? 1 : 0;
        RemotePlayerIndex = LocalPlayerIndex ^ 1;

        PlayerInputSources[0] = pP1InputSource;
        PlayerInputSources[1] = pP2InputSource;

        PlayerInputDevices[0] = p1InputDevice;
        PlayerInputDevices[1] = p2InputDevice;
    }

    uint32_t SampleLocalPlayerDevice() const
    {
        RELEASE_CHECK(LocalPlayerIndex < 2, "Player index out of bound sampling player device");
        return PlayerInputDevices[LocalPlayerIndex]->SampleGameInputState();
    }

    uint32_t GetInputMaskForFrame(uint8_t playerIndex, uint32_t frame)
    {
        size_t latestValidFrame;
        if (!PlayerInputSources[playerIndex]->GetLatestValidFrame(&latestValidFrame))
        {
            // No inputs yet, just return a "predicted" value of zero.
            return 0;
        }

        if (frame > latestValidFrame && playerIndex == RemotePlayerIndex)
        {
            // Provide a predicted result if we're ahead of the last valid input frame
            RELEASE_CHECK(
                LastConfirmedFrame < latestValidFrame,
                "Confirmed frame is larger than latest valid input frame for input source");
            return PlayerInputSources[playerIndex]->GetInputState(latestValidFrame);
        }

        return PlayerInputSources[playerIndex]->GetInputState(frame);
    }

    void SetInputMaskForFrame(uint8_t playerIndex, uint32_t frame, uint32_t inputMask)
    {
        PlayerInputSources[playerIndex]->SetInputState(inputMask, frame);
    }

    static constexpr int kMaxPlayers = 2;
    static constexpr int kMaxPlayerInputFrames = 8;
    IInputDevice* PlayerInputDevices[kMaxPlayers];
    InputSource* PlayerInputSources[kMaxPlayers];
    uint8_t LocalPlayerIndex = 0;
    uint8_t RemotePlayerIndex = 0;

    // Last confirmed frame entry
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
    m_pInputStorage = fixedAllocator.Create<InputStorage>(
        m_pInputSubsystem->GetInputSource(0), m_pInputSubsystem->GetInputSource(1)
    );
    RELEASE_CHECK(m_pInputStorage, "Failed to allocate input storage in LocalGameTickDriver");

    const size_t SendMessageSize = InputsMessage::GetExpectedSize(kNumInputsToSend);
    m_pSendMessage =
        fixedAllocator.ReserveNext<InputsMessage>(SendMessageSize);
    RELEASE_CHECK(m_pSendMessage, "Failed to allocate message send buffer");
    memset(m_pSendMessage, 0, SendMessageSize);
}

void NetworkGameTickDriver::Initialize()
{
    m_serializer.Reset();

    // Initialize the first m_delay frames with 0 from both players
    for (int i = 0; i < m_inputDelay; ++i)
    {
        const uint32_t ClearInputState = 0;
        m_pInputStorage->PlayerInputSources[0]->SetInputState(ClearInputState, i);
        m_pInputStorage->PlayerInputSources[1]->SetInputState(ClearInputState, i);
    }
    m_serializer.SaveFrame(m_inputDelay - 1);
    m_pInputStorage->LastConfirmedFrame = m_inputDelay - 1;
}

void NetworkGameTickDriver::Tick(sputter::math::FixedPoint dt)
{
    const uint32_t CurrentFrame = m_pGameInstance->GetFrame();
    const uint32_t LocalPlayerIndex = m_pInputStorage->LocalPlayerIndex;

    RELEASE_LOGLINE_VERBOSE(LOG_GAME, "TickDriver Ticking frame %u", CurrentFrame);

    m_pInputSubsystem->SetFrame(CurrentFrame);
    m_pInputSubsystem->Tick(dt);

    const uint32_t LocalInputs = m_pInputStorage->SampleLocalPlayerDevice();
    // Sample local input and send "delayframes" into the future
    m_pInputStorage->SetInputMaskForFrame(
        LocalPlayerIndex, CurrentFrame + m_inputDelay, LocalInputs);

    if (!SendNextInputMessage())
    {
        RELEASE_LOGLINE_ERROR(LOG_GAME, "Failed to send inputs to peer");
        // TODO: something more. Are we disconnected?
        return;
    }

    ProcessRollbacks(dt);

    TickOneFrame(
        dt,
        m_pInputStorage->GetInputMaskForFrame(0, CurrentFrame),
        m_pInputStorage->GetInputMaskForFrame(1, CurrentFrame));
    m_serializer.SaveFrame(m_pGameInstance->GetFrame());
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
        m_pReliableUDPSession->Read(
            reinterpret_cast<char *>(pInputMessage),
            MaxMessageLen);
    if (ReadSize <= 0) { return false; }
    if (pInputMessage->Header.MessageSize != InputsMessage::GetExpectedSize(pInputMessage->NumFrames))
    {
        RELEASE_LOGLINE_WARNING(LOG_GAME, "Read unexpected number of bytes for inputs message");
        return false;
    }

    if (pInputMessage->Header.Type != MessageType::Inputs)
    {
        RELEASE_LOGLINE_WARNING(LOG_GAME, "Unexpectedly read non-inputs message");
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
        "Sending - START: %d - NUM: %hhu",
        m_pSendMessage->StartFrame,
        m_pSendMessage->NumFrames);
    const uint8_t LocalPlayerIndex = m_pInputStorage->LocalPlayerIndex;
    for (uint32_t i = 0; i < m_pSendMessage->NumFrames; ++i)
    {
        m_pSendMessage->GameInputMasks[i] =
            m_pInputStorage->GetInputMaskForFrame(LocalPlayerIndex, m_pSendMessage->StartFrame + i);
    }
    const size_t SendMessageSize = InputsMessage::GetExpectedSize(kNumInputsToSend);
    m_pSendMessage->Header.MessageSize = InputsMessage::GetExpectedSize(m_pSendMessage->NumFrames);

    const size_t SentBytes = m_pReliableUDPSession->Send(
        reinterpret_cast<char *>(m_pSendMessage), SendMessageSize);
    if (SentBytes != SendMessageSize)
    {
        RELEASE_LOGLINE_ERROR(LOG_GAME, "Failed to inputs message");
        return false;
    }

    return true;
}

int32_t
NetworkGameTickDriver::ProcessRemoteInputsMessage(InputsMessage *pInputMessage)
{
    int32_t newConfirmedFrame = m_pInputStorage->LastConfirmedFrame;
    int32_t startFrame = -1;
    RELEASE_LOGLINE_VERYVERBOSE(
        LOG_GAME,
        "ProcessMessage++: NumFrames = %d, StartFrame = %d",
        pInputMessage->NumFrames,
        pInputMessage->StartFrame);

    for (uint32_t i = 0; i < pInputMessage->NumFrames; ++i)
    {
        const int32_t Frame = pInputMessage->StartFrame + i;
        if (Frame <= m_pInputStorage->LastConfirmedFrame)
        {
            RELEASE_LOGLINE_VERBOSE(
                LOG_GAME,
                "Skipping frame which predates last confirmed frame: %d <= %d, mask = %u",
                Frame,
                m_pInputStorage->LastConfirmedFrame,
                pInputMessage->GameInputMasks[i]);
            continue;
        }

        const uint8_t RemotePlayerIndex = m_pInputStorage->RemotePlayerIndex;
        const uint32_t RemoteInputForFrame = pInputMessage->GameInputMasks[i];
        if (Frame < m_pGameInstance->GetFrame())
        {
            // Does the provided input differ from our predicted/stored input?
            const bool InputDiffers =
                m_pInputStorage->GetInputMaskForFrame(RemotePlayerIndex, Frame) != RemoteInputForFrame;
            if (InputDiffers && startFrame < 0)
            {
                RELEASE_LOGLINE_VERBOSE(
                    LOG_GAME,
                    "Initializing start frame to %u, mask = %u",
                    Frame,
                    pInputMessage->GameInputMasks[i]);
                startFrame = Frame;
            }
            else if (!InputDiffers && startFrame < 0)
            {
                RELEASE_LOGLINE_VERBOSE(
                    LOG_GAME,
                    "Frame %d was accurately predicted, marking as confirmed",
                    Frame);
                newConfirmedFrame = Frame;
            }
        }

        m_pInputStorage->SetInputMaskForFrame(RemotePlayerIndex, Frame, RemoteInputForFrame);
    }

    if (newConfirmedFrame != m_pInputStorage->LastConfirmedFrame)
    {
        RELEASE_LOGLINE_VERBOSE(
            LOG_GAME,
            "Setting new confirmed frame %d",
            newConfirmedFrame);
        m_pInputStorage->LastConfirmedFrame = newConfirmedFrame;
    }

    RELEASE_LOGLINE_VERYVERBOSE(LOG_GAME, "ProcessMessage--");
    return startFrame;
}

void 
NetworkGameTickDriver::ProcessRollbacks(sputter::math::FixedPoint dt)
{
    bool receivedRemotePlayerMessage = ReadNextRemotePlayerMessage(m_pSendMessage);
    const uint32_t CurrentFrame = m_pGameInstance->GetFrame();
    int32_t rollbackStartFrame = CurrentFrame;
    if (receivedRemotePlayerMessage)
    {
        rollbackStartFrame =
            std::min(ProcessRemoteInputsMessage(m_pSendMessage), rollbackStartFrame);
        receivedRemotePlayerMessage = true;
    }
    else
    {
        RELEASE_LOGLINE_VERBOSE(LOG_GAME, "Did not read any remote input");
        // TODO: something more. Are we disconnected?
    }

    if (!receivedRemotePlayerMessage || CurrentFrame < m_inputDelay)
    {
        TickOneFrame(
            dt,
            m_pInputStorage->GetInputMaskForFrame(0, CurrentFrame),
            m_pInputStorage->GetInputMaskForFrame(1, CurrentFrame));
        return;
    }

    DoRollbacks(dt, rollbackStartFrame);
}

void
NetworkGameTickDriver::DoRollbacks(
    sputter::math::FixedPoint dt,
    int32_t startFrame)
{
    if (startFrame < 0)
    {
        // Nothing to do
        return;
    }

    const uint32_t CurrentFrame = m_pGameInstance->GetFrame();

    RELEASE_CHECK(startFrame >= 0, "Unexpected rollback start frame");
    if (startFrame < CurrentFrame)
    {
        RELEASE_LOGLINE_VERBOSE(LOG_GAME, "Loading frame %d", m_pInputStorage->LastConfirmedFrame);
        m_serializer.LoadFrame(m_pInputStorage->LastConfirmedFrame);
        RELEASE_CHECK(
            m_pGameInstance->GetFrame() == m_pInputStorage->LastConfirmedFrame,
            "Loaded frame is not the expected frame !");
    }

    RELEASE_LOGLINE_VERBOSE(
        LOG_GAME,
        "Beginning rollback: %d -> %d",
        startFrame,
        CurrentFrame - 1);
    for (uint32_t frame = startFrame; frame < CurrentFrame; ++frame)
    {
        RELEASE_LOGLINE_VERYVERBOSE(
            LOG_GAME,
            "Rollback tick: Frame %d",
            frame);
        TickOneFrame(
            dt,
            m_pInputStorage->GetInputMaskForFrame(0, frame),
            m_pInputStorage->GetInputMaskForFrame(1, frame));

        // TODO: evaluate and compare checksums

        // Consider this the last confirmed frame if inputs weren't predicted
        InputSource* pRemoteInputSource = m_pInputStorage->PlayerInputSources[m_pInputStorage->RemotePlayerIndex];
        size_t latestRemoteFrame;
        if (pRemoteInputSource->GetLatestValidFrame(&latestRemoteFrame))
        {
            if (frame <= latestRemoteFrame)
            {
                m_pInputStorage->LastConfirmedFrame = frame;
            }
        }
    }
}
