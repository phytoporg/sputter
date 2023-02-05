#pragma once

#include <sputter/game/tickdriver.h>
#include <sputter/core/serializer.h>

namespace sputter { namespace input {
    class InputSubsystem;
}}

namespace sputter { namespace memory {
    class FixedMemoryAllocator;
}}

class GameInstance;

class LocalGameTickDriver : public sputter::game::ITickDriver
{
public:
    LocalGameTickDriver(
        sputter::memory::FixedMemoryAllocator& fixedAllocator,
        sputter::input::InputSubsystem* pInputSubsystem,
        GameInstance* pGameInstance);

    void Initialize();
    void SetEnableSyncTest(bool enableSyncTest);

    virtual void Tick(sputter::math::FixedPoint dt) override;

private:
    virtual void TickOneFrame(sputter::math::FixedPoint dt, uint32_t p1InputMask, uint32_t p2InputMask) override;

    sputter::input::InputSubsystem* m_pInputSubsystem = nullptr;
    GameInstance*                   m_pGameInstance = nullptr;

    bool                            m_syncTestEnabled = true;

    sputter::core::Serializer       m_serializer;

    // Pimpl this to keep templated containers tidy and unexposed
    struct InputStorage;
    InputStorage*                   m_pInputStorage = nullptr;
};