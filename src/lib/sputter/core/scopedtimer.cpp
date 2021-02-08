#include "scopedtimer.h"

#include <sputter/system/time.h>

namespace sputter { namespace core {
    ScopedTimer::ScopedTimer(uint32_t* pElapsedMs)
        : m_startTime(GetTickMilliseconds()),
          m_pElapsedMs(pElapsedMs)
    {}

    ScopedTimer::~ScopedTimer()
    {
        *m_pElapsedMs = GetTickMilliseconds() - m_startTime;
    }
}}
