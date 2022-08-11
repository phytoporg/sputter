#include "inputsource.h"

using namespace sputter::input;

InputSource::InputSource(IInputDevice* pInputDevice) 
    : m_pInputDevice(pInputDevice)
{}

uint32_t InputSource::GetInputState() const 
{
    // TODO
    return 0;
}

uint32_t InputSource::GetPreviousState() const 
{
    // TODO
    return 0;
}

bool InputSource::IsInputHeld(uint32_t gameInputCode) const 
{
    // TODO
    return false;
}

bool InputSource::IsInputReleased(uint32_t gameInputCode) const 
{
    // TODO
    return false;
}

bool InputSource::IsInputPressed(uint32_t gameInputCode) const 
{
    // TODO
    return false;
}

void InputSource::Tick() 
{
    // TODO
    return;    
}
