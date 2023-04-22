#include "remotedevice.h"

using namespace sputter;
using namespace sputter::input;

RemoteDevice::RemoteDevice(net::ReliableUDPSession *pReliableUDPSession)
    : m_pReliableUDPSession(pReliableUDPSession)
{}

DeviceType RemoteDevice::GetDeviceType() const
{
    return DeviceType::Remote;
}

uint32_t RemoteDevice::SampleGameInputState() const
{
    return 0;
}

uint32_t RemoteDevice::SampleGameInputState(uint32_t gameCode) const
{
    return 0;
}

InputDeviceConnectionState RemoteDevice::GetConnectionState() const
{
    // TODO: return actual reliable connection state
    return InputDeviceConnectionState::Invalid;
}

void RemoteDevice::SetInputMap(const InputMapEntry *pEntries, size_t numEntries)
{
    // NOOP?
}

void RemoteDevice::Tick()
{
    IInputDevice::Tick();
    if (m_pReliableUDPSession)
    {
        m_pReliableUDPSession->Tick();
    }
}
