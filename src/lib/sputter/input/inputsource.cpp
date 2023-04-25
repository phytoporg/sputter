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

void InputSource::SetInputState(uint32_t inputState, uint32_t frame)
{
    if (frame == kCurrentFrame) { frame = CurrentFrame; }
    if (m_inputStateBuffer.size() <= frame)
    {
        m_inputStateBuffer.resize(frame + 1);
    }
    RELEASE_CHECK(frame < m_inputStateBuffer.size() || frame == kCurrentFrame, "Sampling invalid frame");
    m_inputStateBuffer[frame] = inputState;
}

uint32_t InputSource::GetInputState(uint32_t frame) const 
{
    if (frame == kCurrentFrame) { frame = CurrentFrame; }

    RELEASE_CHECK(frame < m_inputStateBuffer.size() || frame == kCurrentFrame, "Sampling invalid frame");
    return m_inputStateBuffer[frame];
}

bool InputSource::GetLatestValidFrame(size_t* pLatestFrameOut) const
{
    if (m_inputStateBuffer.empty())
    {
        return false;
    }

    *pLatestFrameOut = m_inputStateBuffer.size() - 1;
    return true;
}

bool InputSource::IsInputHeld(uint32_t gameInputCode, uint32_t frame) const 
{
    if (frame == kCurrentFrame) { frame = CurrentFrame; }
    if (frame < 1) { return false; } // No previous frame

    const uint32_t BitMask = (1 << gameInputCode);
    const uint32_t Current = GetInputState(frame);
    const uint32_t Previous = GetInputState(frame - 1);
    return (Current & BitMask) && (Previous & BitMask);
}

bool InputSource::IsInputReleased(uint32_t gameInputCode, uint32_t frame) const 
{
    if (frame == kCurrentFrame) { frame = CurrentFrame; }
    if (frame < 1) { return false; } // No previous frame

    const uint32_t BitMask = (1 << gameInputCode);
    const uint32_t Current = GetInputState(frame);
    const uint32_t Previous = GetInputState(frame - 1);
    return !(Current & BitMask) && (Previous & BitMask);
}

bool InputSource::IsInputPressed(uint32_t gameInputCode, uint32_t frame) const 
{
    if (frame == kCurrentFrame) { frame = CurrentFrame; }

    const uint32_t BitMask = (1 << gameInputCode);
    return GetInputState(frame) & BitMask;
}

void InputSource::SetFrame(uint32_t frame)
{
    if (m_inputStateBuffer.size() <= frame)
    {
        m_inputStateBuffer.resize(frame + 1, 0);
    }

    CurrentFrame = frame;
}

void InputSource::Reset()
{
    std::fill(std::begin(m_inputStateBuffer), std::end(m_inputStateBuffer), 0);
    m_inputStateBuffer.resize(1);
    CurrentFrame = 0;
}

void InputSource::Tick()
{
    if (m_pInputDevice)
    {
        m_pInputDevice->Tick();
    }
}
