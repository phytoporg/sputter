#include "timersystem.h"

#include <utility>

using namespace sputter::game;
using TimerHandle = TimerSystem::TimerHandle;

bool TimerSystem::Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes)
{
    size_t bytesWritten = 0;

    WRITE(m_entries, pBuffer, bytesWritten, maxBytes);
    bytesWritten += sizeof(m_entries);

    WRITE(m_nextTimerHandle, pBuffer, bytesWritten, maxBytes);
    bytesWritten += sizeof(m_nextTimerHandle);

    WRITE(m_nextEntry, pBuffer, bytesWritten, maxBytes);
    bytesWritten += sizeof(m_nextEntry);

    *pBytesWrittenOut += bytesWritten;

    return true;
}

bool TimerSystem::Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes)
{
    size_t bytesRead = 0;

    READ(m_entries, pBuffer, bytesRead, maxBytes);
    bytesRead += sizeof(m_entries);

    READ(m_nextTimerHandle, pBuffer, bytesRead, maxBytes);
    bytesRead += sizeof(m_nextTimerHandle);

    READ(m_nextEntry, pBuffer, bytesRead, maxBytes);
    bytesRead += sizeof(m_nextEntry);

    *pBytesReadOut += bytesRead;

    return true;
}

TimerHandle TimerSystem::CreateFrameTimer(uint32_t numFrames, void (*pfnCallback)(void*), void* pUserData)
{
    sputter::core::Functor onTimerExpired(reinterpret_cast<uintptr_t>(pfnCallback), pUserData);
    return CreateLoopingFrameTimer(numFrames, 1, onTimerExpired);
}

TimerHandle TimerSystem::CreateLoopingFrameTimer(uint32_t numFrames, int8_t loopCount, void (*pfnCallback)(void*), void* pUserData)
{
    sputter::core::Functor onTimerExpired(reinterpret_cast<uintptr_t>(pfnCallback), pUserData);
    return CreateLoopingFrameTimer(numFrames, loopCount, onTimerExpired);
}

TimerHandle TimerSystem::CreateFrameTimer(uint32_t numFrames, const core::Functor& onTimerExpired)
{
    return CreateLoopingFrameTimer(numFrames, 1, onTimerExpired);
}

TimerHandle TimerSystem::CreateLoopingFrameTimer(uint32_t numFrames, int8_t loopCount, const core::Functor& onTimerExpired)
{
    if (m_nextEntry >= kMaxTimerEntries || numFrames == 0)
    {
        return kInvalidTimerHandle;
    }

    TimerEntry& entry = m_entries[m_nextEntry];
    entry.Handle = m_nextTimerHandle;
    entry.OnTimerExpired = onTimerExpired;
    entry.InitialFrames = numFrames;
    entry.FramesRemaining = numFrames;
    entry.LoopCount = loopCount;

    m_nextEntry = (m_nextEntry + 1) % kMaxTimerEntries;
    ++m_nextTimerHandle;

    return entry.Handle;
}

bool TimerSystem::ClearTimer(TimerHandle timerHandle)
{
    if (timerHandle == kInvalidTimerHandle)
    {
        return false;
    }

    for (uint8_t i = 0; i < m_nextEntry; ++i)
    {
        if (m_entries[i].Handle == timerHandle)
        {
            if (i < (m_nextEntry - 1))
            {
                std::swap(m_entries[i], m_entries[m_nextEntry - 1]);
            }

            m_entries[i].Handle = kInvalidTimerHandle;
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
            if (entry.FramesRemaining == 0 && entry.Handle != kInvalidTimerHandle)
            {
                if (entry.OnTimerExpired)
                {
                    entry.OnTimerExpired();
                }

                // The callback may have cleared the timer. Check before proceeding!
                if (entry.Handle == kInvalidTimerHandle)
                {
                    continue;
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
                else if (entry.LoopCount > 0)
                {
                    // More loops to go, reset frames remaining
                    entry.FramesRemaining = entry.InitialFrames;
                }
            }
        }
    }
}
