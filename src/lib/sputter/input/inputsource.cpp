#include "inputsource.h"
#include "inputdevice.h"

#include <sputter/core/check.h>

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

uint32_t InputSource::GetInputState(uint32_t frame) const 
{
    if (frame == kCurrentFrame) { frame = CurrentFrame; }

    RELEASE_CHECK(frame < m_inputStateBuffer.size() || frame == kCurrentFrame, "Sampling invalid frame");
    return m_inputStateBuffer[frame];
}

uint32_t InputSource::GetPreviousState(uint32_t frame) const 
{
    if (frame == kCurrentFrame) { frame = CurrentFrame; }
    if (frame == 0) { return 0; } // No previous frame on frame 0

    RELEASE_CHECK(frame < m_inputStateBuffer.size() || frame == kCurrentFrame, "Sampling invalid frame");
    return m_inputStateBuffer[frame - 1];
}

bool InputSource::IsInputHeld(uint32_t gameInputCode, uint32_t frame) const 
{
    if (frame == kCurrentFrame) { frame = CurrentFrame; }

    const uint32_t BitMask = (1 << gameInputCode);
    return (GetInputState(frame) & BitMask) && (GetPreviousState(frame) & BitMask);
}

bool InputSource::IsInputReleased(uint32_t gameInputCode, uint32_t frame) const 
{
    const uint32_t BitMask = (1 << gameInputCode);
    return !(GetInputState(frame) & BitMask) && (GetPreviousState(frame) & BitMask);
}

bool InputSource::IsInputPressed(uint32_t gameInputCode, uint32_t frame) const 
{
    const uint32_t BitMask = (1 << gameInputCode);
    return GetInputState(frame) & BitMask;
}

void InputSource::Tick() 
{
    RELEASE_CHECK(CurrentFrame < m_inputStateBuffer.size(), "Input buffer am too small !!");
    if (m_pInputDevice)
    {
        m_inputStateBuffer[CurrentFrame] = m_pInputDevice->SampleGameInputState();
    }
}

void InputSource::SetFrame(uint32_t frame)
{
    if (m_inputStateBuffer.size() <= frame)
    {
        m_inputStateBuffer.resize(frame + 1, 0);
    }

    CurrentFrame = frame;
}
