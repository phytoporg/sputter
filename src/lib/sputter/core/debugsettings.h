#pragma once

#include <cstdint>

namespace sputter { namespace core { namespace debugsettings {
    static constexpr uint32_t kMaxSettingNameLength = 128;
    void SetDebugSetting(const char* pSettingName, bool value);
    bool GetDebugSetting(const char* pSettingName);
}}}
