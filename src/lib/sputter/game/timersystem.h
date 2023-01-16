#pragma once

#include <cstdint>
#include <sputter/core/functor.h>

namespace sputter { namespace game {
    // Booo, I don't like this name but "TimerManager" is worse
    class TimerSystem
    {
    public:
        using TimerHandle = uint32_t;
        static const TimerHandle kInvalidTimerHandle = 0xFFFFFFFF;

        TimerHandle CreateFrameTimer(uint32_t numFrames, void (*pfnCallback)(void*) = nullptr, void* pUserData = nullptr);
        TimerHandle CreateLoopingFrameTimer(uint32_t numFrames, int8_t loopCount, void (*pfnCallback)(void*) = nullptr, void* pUserData = nullptr);

        TimerHandle CreateFrameTimer(uint32_t numFrames, const core::Functor& onTimerExpired = core::Functor());
        TimerHandle CreateLoopingFrameTimer(uint32_t numFrames, int8_t loopCount, const core::Functor& onTimerExpired = core::Functor());
        bool ClearTimer(TimerHandle timerHandle);

        void Tick();

    private:
        struct TimerEntry
        {
            TimerHandle Handle = kInvalidTimerHandle;
            core::Functor OnTimerExpired;
            uint64_t InitialFrames = 0;
            uint64_t FramesRemaining = 0;
            int8_t LoopCount = -1; 
        };

        static const uint8_t kMaxTimerEntries = 255;

        TimerEntry  m_entries[kMaxTimerEntries];
        TimerHandle m_nextTimerHandle = 0;
        uint8_t     m_nextEntry = 0;
    };
}}