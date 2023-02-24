#include "port.h"

// LINUX++
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
// LINUX--

#include <sputter/core/check.h>
#include <sputter/log/log.h>
#include <sputter/system/system.h>

using namespace sputter;
using namespace sputter::net;

UDPPort::UDPPort(uint32_t port)
{
    m_socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socketHandle < 0)
    {
        system::LogAndFail("Failed to create socket");
    }

    // Store port address
    sockaddr_in bindAddress = {};
    bindAddress.sin_family = AF_INET;
    bindAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    bindAddress.sin_port = htons(port);
    if (bind(m_socketHandle, reinterpret_cast<sockaddr *>(&bindAddress), sizeof(bindAddress)) < 0) 
    {
        system::LogAndFail("Failed to create socket");
    }

    m_port = port;
}

UDPPort::~UDPPort()
{
    if (m_socketHandle >= 0) 
    {
        ::close(m_socketHandle);
        m_socketHandle = -1;
    }
}

bool UDPPort::send(const void *data, int dataSize, const std::string &address) const
{
    RELEASE_CHECK(m_socketHandle >= 0, "Socket is not open");

    sockaddr_in dest = {};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(m_port);
    if (inet_pton(AF_INET, address.c_str(), &dest.sin_addr) != 1) 
    {
        RELEASE_LOGLINE_WARNING(LOG_NET, "Failed to convert address to binary (address = %s)", address.data());
        return false;
    }

#if DEBUG
    // Print the address and port we're sending to
    char addressBuffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &dest.sin_addr, addressBuffer, sizeof(addressBuffer));
    DEBUG_LOGLINE_INFO(LOG_NET, "Sending to %s: %d", addressBuffer, m_port);
#endif

    const int sent = sendto(m_socketHandle, data, dataSize, 0, reinterpret_cast<sockaddr *>(&dest), sizeof(dest));
    if (sent < 0) 
    {
        RELEASE_LOGLINE_WARNING(LOG_NET, "Failed to send data");
        return false;
    } 
    else 
    {
        DEBUG_LOGLINE_INFO(LOG_NET, "Sent %d bytes", sent);
        return true;
    }
}

int UDPPort::receive(void *data, int dataSize, std::string* pAddressOut) const
{
    RELEASE_CHECK(m_socketHandle >= 0, "Socket is not open");

    sockaddr_in src = {};
    socklen_t srcLength;
    const int received = recvfrom(m_socketHandle, data, dataSize, 0, reinterpret_cast<sockaddr *>(&src), &srcLength);
    if (received < 0) 
    {
        RELEASE_LOGLINE_WARNING(LOG_NET, "Failed to receive data");
        return -1;
    }

    if (pAddressOut)
    {
        // Convert address to string and assign to *pAddressOut
        char addressBuffer[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &src.sin_addr, addressBuffer, sizeof(addressBuffer)) == nullptr) 
        {
            RELEASE_LOGLINE_ERROR(LOG_NET, "Failed to convert address to string");
            return -1;
        }
        *pAddressOut = addressBuffer;
    }

    return received;
}

int UDPPort::GetPort() const
{
    return m_port;
}