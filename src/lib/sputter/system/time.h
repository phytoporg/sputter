#pragma once

#include <cstdint>

namespace sputter { namespace system {
    uint32_t GetTickMilliseconds();
	void SleepMs(uint32_t milliseconds);
}}
