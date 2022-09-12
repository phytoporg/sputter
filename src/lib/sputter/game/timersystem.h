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
        TimerHandle CreateFrameTimer(uint32_t numFrames, TimerExpiryFunctor pfnOnExpiry = nullptr, void* pUserData = nullptr);
        TimerHandle CreateLoopingFrameTimer(uint32_t numFrames, int8_t loopCount, TimerExpiryFunctor pfnOnExpiry = nullptr, void* pUserData = nullptr);
        bool ClearTimer(TimerHandle timerHandle);

        void Tick();

    private:
        struct TimerEntry
        {
            TimerHandle Handle = kInvalidTimerHandle;
            TimerExpiryFunctor pfnExpiryCallback = nullptr;
            void* pUserData = nullptr;
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