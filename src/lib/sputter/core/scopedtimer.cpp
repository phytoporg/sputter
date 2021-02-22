#include "scopedtimer.h"

#include <sputter/system/time.h>

namespace sputter { namespace core {
    ScopedTimer::ScopedTimer(uint32_t* pElapsedMs)
        : m_startTime(system::GetTickMilliseconds()),
          m_pElapsedMs(pElapsedMs)
    {}

    ScopedTimer::~ScopedTimer()
    {
        *m_pElapsedMs = system::GetTickMilliseconds() - m_startTime;
    }
}}
