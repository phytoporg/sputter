#pragma once

#include <cstdint>

namespace sputter { namespace game {
    // Booo, I don't like this name but "TimerManager" is worse
    class TimerSystem
    {
    public:
        using TimerHandle = uint32_t;
        using TimerExpiryFunctor = void(*)(TimerSystem*, TimerHandle, void*);
        static const TimerHandle kInvalidTimerHandle = 0xFFFFFFFF;

        // TODO: How are functors supposed to be serialized?
        TimerHandle CreateFrameTimer(uint32_t numFrames, TimerExpiryFunctor pfnOnExpiry);
        bool DestroyTimer(TimerHandle timerHandle);

    private:
        struct TimerEntry
        {
            TimerHandle Handle = kInvalidTimerHandle;
            uint64_t FramesRemaining = 0;
        };

        TimerHandle m_nextTimerHandle = 0;
        static const uint8_t kMaxTimerEntries = 256;
        TimerEntry m_entries[kMaxTimerEntries];
    };
}}