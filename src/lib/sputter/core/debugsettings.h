#pragma once

namespace sputter { namespace core { namespace debugsettings {
    void SetDebugSetting(const char* pSettingName, bool value);
    bool GetDebugSetting(const char* pSettingName);
}}}
