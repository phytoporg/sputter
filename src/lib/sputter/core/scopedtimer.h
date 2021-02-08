#pragma once

namespace sputter { namespace core {
    class ScopedTimer
    {
    public:
        ScopedTimer(uint32_t* pElapsedMs);
        ~ScopedTimer();

    private:
        uint32_t  m_startTime;
        uint32_t* m_pElapsedMs;
    };
}}
