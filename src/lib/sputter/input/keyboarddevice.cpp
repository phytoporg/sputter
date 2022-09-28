#include "keyboarddevice.h"
#include <sputter/render/window.h>

#include <sputter/core/check.h>

using namespace sputter::input;

// TODO: This whole abstraction is ridiculous

KeyboardDevice::KeyboardDevice(sputter::render::Window* pWindow)
    : m_pWindow(pWindow)
{}

uint32_t KeyboardDevice::SampleGameInputState() const
{
    uint32_t bitFlags = 0;
    for (size_t i = 0; i < m_inputMap.size(); ++i)
    {
        if (m_pWindow->GetKeyState(m_inputMap[i].DeviceInputCode))
        {
            bitFlags |= (1 << m_inputMap[i].GameInputCode);
        }
    }

    return bitFlags;
}

uint32_t KeyboardDevice::SampleGameInputState(uint32_t code) const
{
    for (size_t i = 0; i < m_inputMap.size(); ++i)
    {
        if (m_inputMap[i].GameInputCode == code)
        {
            return m_pWindow->GetKeyState(m_inputMap[i].DeviceInputCode);
        }
    }

    return 0;
}

InputDeviceConnectionState KeyboardDevice::GetConnectionState() const
{
    // Assuming there's always a keyboard
    return InputDeviceConnectionState::Connected;
}

void KeyboardDevice::SetInputMap(InputMapEntry const* pEntries, size_t numEntries)
{
    m_inputMap.resize(numEntries);
    for (size_t i = 0; i < numEntries; ++i)
    {
        m_inputMap[i] = {
            pEntries[i].DeviceInputCode,
            pEntries[i].GameInputCode
            };
    }
}