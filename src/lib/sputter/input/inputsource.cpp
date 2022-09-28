#include "inputsource.h"
#include "inputdevice.h"

using namespace sputter::input;

InputSource::InputSource(IInputDevice* pInputDevice) 
    : m_pInputDevice(pInputDevice)
{}

InputSource::~InputSource()
{}

IInputDevice* InputSource::GetInputDevice()
{
    return m_pInputDevice;
}

uint32_t InputSource::GetInputState() const 
{
    if (!m_pInputDevice)
    {
        return m_pInputDevice->SampleGameInputState();
    }
    
    return 0;
}

uint32_t InputSource::GetPreviousState() const 
{
    return m_previousInput;
}

bool InputSource::IsInputHeld(uint32_t gameInputCode) const 
{
    if (m_pInputDevice)
    {
        const uint32_t bitMask = (1 << gameInputCode);
        return (m_currentInput & bitMask) && (m_previousInput & bitMask);
    }

    return false;
}

bool InputSource::IsInputReleased(uint32_t gameInputCode) const 
{
    if (m_pInputDevice)
    {
        const uint32_t bitMask = (1 << gameInputCode);
        return !(m_currentInput & bitMask) && (m_previousInput & bitMask);
    }

    return false;
}

bool InputSource::IsInputPressed(uint32_t gameInputCode) const 
{
    if (m_pInputDevice)
    {
        return m_pInputDevice->SampleGameInputState(gameInputCode);
    }

    return false;
}

void InputSource::Tick() 
{
    if (m_pInputDevice)  
    {
        m_previousInput = m_currentInput;
        m_currentInput = m_pInputDevice->SampleGameInputState();
    }
}
