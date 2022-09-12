#include "timersystem.h"

#include <utility>

using namespace sputter::game;
using TimerHandle = TimerSystem::TimerHandle;

TimerHandle TimerSystem::CreateFrameTimer(uint32_t numFrames, TimerExpiryFunctor pfnOnExpiry, void* pUserData)
{
    return CreateLoopingFrameTimer(numFrames, 1, pfnOnExpiry, pUserData);
}

TimerHandle TimerSystem::CreateLoopingFrameTimer(uint32_t numFrames, int8_t loopCount, TimerExpiryFunctor pfnOnExpiry, void* pUserData)
{
    if (m_nextEntry >= kMaxTimerEntries || numFrames == 0)
    {
        return kInvalidTimerHandle;
    }

    TimerEntry& entry = m_entries[m_nextEntry];
    entry.Handle = m_nextTimerHandle;
    entry.pfnExpiryCallback = pfnOnExpiry;
    entry.pUserData = pUserData;
    entry.InitialFrames = numFrames;
    entry.FramesRemaining = numFrames;
    entry.LoopCount = loopCount;

    m_nextEntry = (m_nextEntry + 1) % kMaxTimerEntries;
    ++m_nextTimerHandle;

    return entry.Handle;
}

bool TimerSystem::ClearTimer(TimerHandle timerHandle)
{
    for (uint8_t i = 0; i < m_nextEntry; ++i)
    {
        if (m_entries[i].Handle == timerHandle)
        {
            if (i < (m_nextEntry - 1))
            {
                std::swap(m_entries[i], m_entries[m_nextEntry - 1]);
            }

            --m_nextEntry;
            return true;
        }
    }

    return false;
}

void TimerSystem::Tick()
{
    for (uint8_t i = 0; i < m_nextEntry; ++i)
    {
        TimerEntry& entry = m_entries[i];
        if (entry.FramesRemaining > 0)
        {
            entry.FramesRemaining--;
            if (entry.FramesRemaining == 0)
            {
                if (entry.FramesRemaining == 0 && entry.pfnExpiryCallback)
                {
                    entry.pfnExpiryCallback(this, entry.Handle, entry.pUserData);
                }

                if (entry.LoopCount > 0)
                {
                    entry.LoopCount--;
                }

                if (entry.LoopCount == 0)
                {
                    // Retire the timer
                    entry.Handle = kInvalidTimerHandle;
                    if (i < (m_nextEntry - 1))
                    {
                        std::swap(entry, m_entries[m_nextEntry - 1]);
                    }
                    m_nextEntry--;
                }
                else
                {
                    // More loops to go, reset frames remaining
                    entry.FramesRemaining = entry.InitialFrames;
                }
            }
        }
    }
}
