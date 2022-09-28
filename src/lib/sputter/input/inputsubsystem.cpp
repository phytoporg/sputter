#include "inputsubsystem.h"
#include "keyboarddevice.h"

#include <sputter/render/window.h>

#include <sputter/system/system.h>

using namespace sputter::input;

InputSubsystem::InputSubsystem(const InputSubsystemSettings& settings)
{
    for (int i = 0; i < 2; ++i)
    {
        IInputDevice* pInputDevice = nullptr;
        if (settings.PlayerDevices[i] == DeviceType::KeyboardInputDevice &&
            settings.pWindow)
        {
            pInputDevice = new KeyboardDevice(settings.pWindow);
        }
        else if (settings.PlayerDevices[i] != DeviceType::None)
        {
            system::LogAndFail("Unsupported input device");
        }

        m_inputSources.emplace_back(pInputDevice);

        if (settings.pInputMapEntryArrays[i])
        {
            IInputDevice* pInputDevice = m_inputSources[i].GetInputDevice();
            pInputDevice->SetInputMap(
                settings.pInputMapEntryArrays[i],
                settings.pNumInputMapEntries[i]);
        }
    }
}

void InputSubsystem::Tick(math::FixedPoint dt)
{
    glfwPollEvents();
    for (size_t i = 0; i < 2; ++i)
    {
        m_inputSources[i].Tick();
    }
}

InputSource* InputSubsystem::CreateComponent(const InputSource::InitializationParameters& params)
{
    if (params.PlayerId <= 2 && params.PlayerId >= 0)
    {
        return &m_inputSources[params.PlayerId];
    }

    return nullptr;
}

void InputSubsystem::ReleaseComponent(InputSource* pInputSource)
{
}

const InputSource* InputSubsystem::GetInputSource(uint8_t playerIndex) const
{
    if (playerIndex < m_inputSources.size())
    {
        return &m_inputSources[playerIndex];
    }

    system::LogAndFail("Player index out of bounds.");
    return nullptr; // Appease the compiler
}
