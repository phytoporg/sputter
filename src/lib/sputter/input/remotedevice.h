#pragma once

#include "inputdevice.h"

namespace sputter
{
    namespace net
    {
        class ReliableUDPSession;
    }
}

namespace sputter { namespace input {
    class RemoteDevice : public IInputDevice
    {
    public:
        RemoteDevice(net::ReliableUDPSession* pReliableUDPSession);

        virtual DeviceType GetDeviceType() const override;
        virtual uint32_t SampleGameInputState() const override;
        virtual uint32_t SampleGameInputState(uint32_t gameCode) const override;
        virtual InputDeviceConnectionState GetConnectionState() const override;
        virtual void SetInputMap(InputMapEntry const* pEntries, size_t numEntries) override;
        virtual void Tick() override;

    private:
        net::ReliableUDPSession* m_pReliableUDPSession = nullptr;
    };
}}