#pragma once

#include <sputter/game/tickdriver.h>
#include <sputter/core/serializer.h>
#include "protocol.h"

namespace sputter { namespace input {
    class InputSubsystem;
}}

namespace sputter { namespace memory {
    class FixedMemoryAllocator;
}}

namespace sputter { namespace net {
    class ReliableUDPSession;
}}

class GameInstance;

class NetworkGameTickDriver : public sputter::game::ITickDriver
{
public:
    NetworkGameTickDriver(
        sputter::memory::FixedMemoryAllocator& fixedAllocator,
        sputter::input::InputSubsystem* pInputSubsystem,
        sputter::net::ReliableUDPSession* pReliableUDPSession,
        GameInstance* pGameInstance);

    virtual void Initialize() override;
    virtual void Tick(sputter::math::FixedPoint dt) override;

private:
    virtual void TickOneFrame(
        sputter::math::FixedPoint dt,
        uint32_t p1InputMask,
        uint32_t p2InputMask) override;

    bool ReadNextRemotePlayerMessage(InputsMessage* pInputMessage);
    bool SendNextInputMessage() const;

    int32_t ProcessRemoteInputsMessage(InputsMessage* pInputMessage);
    void DoRollbacks(
        sputter::math::FixedPoint dt,
        int32_t startFrame);

    uint32_t                          m_inputDelay = 3; // Hardcode for now

    sputter::input::InputSubsystem*   m_pInputSubsystem = nullptr;
    GameInstance*                     m_pGameInstance = nullptr;
    sputter::net::ReliableUDPSession* m_pReliableUDPSession = nullptr;

    sputter::core::Serializer         m_serializer;

    struct InputStorage;
    InputStorage*                     m_pInputStorage = nullptr;

    uint32_t*                         m_pInputDelayBuffer = nullptr;

    static constexpr uint32_t         kNumInputsToSend = 8; // TODO: what's good???
    InputsMessage*                    m_pSendMessage = nullptr;
};
