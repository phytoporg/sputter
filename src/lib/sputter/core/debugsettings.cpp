#include "debugsettings.h"
#include "check.h"

#include <cstdint>
#include <cstring>

namespace debugsettings = sputter::core::debugsettings;

#if !defined(DEBUG)
    void debugsettings::SetDebugSetting(const char* pSettingName, bool value) {}
    bool debugsettings::GetDebugSetting(const char* pSettingName) { return false; } 
#else
    static const uint32_t kNumMaxSettings = 8;

    struct SettingNameBooleanPair
    {
        const char* pName = nullptr;
        bool Value = false;
    };
    static SettingNameBooleanPair BooleanSettings[kNumMaxSettings] = {};
    uint32_t NumBooleanSettings = 0;

    static int32_t _FindBooleanSetting(const char* pName)
    {
        for (int32_t i = 0; i < NumBooleanSettings; ++i)
        {
            const SettingNameBooleanPair& CurrentPair = BooleanSettings[i];
            if (!CurrentPair.pName)
            {
                return -1;
            }
            else if (strncmp(pName, CurrentPair.pName) == 0)
            {
                return i;
            }
        }

        return -1;
    }

    void debugsettings::SetDebugSetting(const char* pSettingName, bool value)
    {
        int32_t settingIndex = _FindBooleanSetting(pSettingName);
        if (settingIndex < 0)
        {
            if (NumBooleanSettings < kNumMaxSettings)
            {
                // Create a new setting
                settingIndex = NumBooleanSettings;
                ++NumBooleanSettings;

                BooleanSettings[i].pName = pSettingName;
            }
            else
            {
                system::LogAndFail("Hit max number of boolean settings");
            }
        }
        
        if (settingIndex >= 0)
        {
            const SettingNameBooleanPair& CurrentPair = BooleanSettings[i];
            CurrentPair.Value = value;
        }
    }

    bool debugsettings::GetDebugSetting(const char* pSettingName)
    {
        int32_t settingIndex = _FindBooleanSetting(pSettingName);
        if (settingIndex < 0)
        {
            return false;
        }
        
        if (settingIndex >= 0)
        {
            CHECK_RELEASE(settingIndex < NumBooleanSettings, "Index out of bounds");
            return BooleanSettings[settingIndex].Value;
        }

        return false;
    }
#endif

