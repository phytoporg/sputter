#include "port.h"

// LINUX++
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
// LINUX--

#include <cstring>

#include <sputter/core/check.h>
#include <sputter/log/log.h>
#include <sputter/system/system.h>

using namespace sputter;
using namespace sputter::net;

UDPPort::UDPPort(int port)
{
    m_socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socketHandle < 0)
    {
        system::LogAndFail("Failed to create socket");
    }

    if (fcntl(m_socketHandle, F_SETFL, O_NONBLOCK) < 0)
    {
        system::LogAndFail("Failed to set nonblocking flag for socket handle");
    }

    m_port = port;
}

UDPPort::UDPPort(const UDPPort &port)
    : m_socketHandle(port.m_socketHandle),
      m_port(port.m_port),
      m_remoteAddress(port.m_remoteAddress),
      m_remotePort(port.m_remotePort)
{}

UDPPort::~UDPPort()
{
    if (m_socketHandle >= 0) 
    {
        ::close(m_socketHandle);
        m_socketHandle = -1;
    }
}

bool UDPPort::bind()
{
    // Store port address
    sockaddr_in bindAddress = {};
    bindAddress.sin_family = AF_INET;
    bindAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    bindAddress.sin_port = htons(m_port);
    if (::bind(m_socketHandle, reinterpret_cast<sockaddr *>(&bindAddress), sizeof(bindAddress)) < 0)
    {
        RELEASE_LOGLINE_ERROR(LOG_NET, "Failed to bind to socket: %d (%s)", m_port, strerror(errno));
        return false;
    }

    RELEASE_LOGLINE_INFO(LOG_NET, "Port bound to port: %d", m_port);
    return true;
}

bool UDPPort::connect(const std::string &address, int remotePort)
{
    struct addrinfo info = {};
    struct addrinfo* pRes = {};

    char portStringBuffer[6] = {};
    sprintf(portStringBuffer, "%d", remotePort);
    if (getaddrinfo(address.c_str(), portStringBuffer, &info, &pRes) != 0)
    {
        RELEASE_LOGLINE_ERROR(
                LOG_NET,
                "Could not retrieve address info for %s:%s",
                address.c_str(), portStringBuffer);
        return false;
    }

    if (::connect(m_socketHandle, pRes->ai_addr, pRes->ai_addrlen) < 0)
    {
        RELEASE_LOGLINE_ERROR(
                LOG_NET,
                "Failed to connect to %s:%s",
                address.c_str(), portStringBuffer);
        return false;
    }

    RELEASE_LOGLINE_INFO(
            LOG_NET,
            "Successfully connected to %s:%s",
            address.c_str(), portStringBuffer);
    m_remoteAddress = address;
    m_remotePort = remotePort;

    return true;
}

int UDPPort::send(const void *data, int dataSize, const std::string& address, int port) const
{
    RELEASE_CHECK(m_socketHandle >= 0, "Socket is not open");

    sockaddr_in dest = {};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    if (inet_pton(AF_INET, address.c_str(), &dest.sin_addr) <= 0)
    {
        RELEASE_LOGLINE_WARNING(LOG_NET, "Failed to convert address to binary (address = %s)", address.data());
        return -1;
    }

#if DEBUG
    // Print the address and port we're sending to
    char addressBuffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &dest.sin_addr, addressBuffer, sizeof(addressBuffer));
    DEBUG_LOGLINE_VERBOSE(LOG_NET, "Sending to %s: %d", addressBuffer, port);
#endif

    const int sent = sendto(m_socketHandle, data, dataSize, 0, reinterpret_cast<sockaddr *>(&dest), sizeof(dest));
    if (sent < 0) 
    {
        RELEASE_LOGLINE_WARNING(LOG_NET, "Failed to send data: %s", strerror(errno));
        return -1;
    } 
    else 
    {
        DEBUG_LOGLINE_VERBOSE(LOG_NET, "Sent %d bytes", sent);
        return sent;
    }
}

int UDPPort::receive(void *data, int dataSize, std::string* pAddressOut, int* pPortOut) const
{
    RELEASE_CHECK(m_socketHandle >= 0, "Socket is not open");

    fd_set readFds;
    FD_ZERO(&readFds);
    FD_SET(m_socketHandle, &readFds);

    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    const int SelectValue = select(m_socketHandle + 1, &readFds, nullptr, nullptr, &tv);
    if (SelectValue < 0)
    {
        RELEASE_LOGLINE_ERROR(LOG_NET, "select() failed on socket: %s", strerror(errno));
        return -1;
    }

    if (!SelectValue)
    {
        return 0;
    }

    sockaddr_in src = {};
    socklen_t srcLength = sizeof(src);
    const int received = recvfrom(m_socketHandle, data, dataSize, 0, reinterpret_cast<sockaddr *>(&src), &srcLength);
    if (received < 0) 
    {
        RELEASE_LOGLINE_WARNING(LOG_NET, "Failed to receive data");
        return -1;
    }
    RELEASE_LOGLINE_VERBOSE(LOG_NET, "Received %d bytes", received);

    if (pAddressOut)
    {
        // Convert address to string and assign to *pAddressOut
        char addressBuffer[INET_ADDRSTRLEN] = {};
        if (inet_ntop(AF_INET, &src.sin_addr, addressBuffer, sizeof(addressBuffer)) == nullptr) 
        {
            RELEASE_LOGLINE_ERROR(LOG_NET, "Failed to convert address to string");
            return -1;
        }
        *pAddressOut = addressBuffer;
    }

    if (pPortOut)
    {
        *pPortOut = htons(src.sin_port);
    }

    return received;
}

int UDPPort::GetSocket() const
{
    return m_socketHandle;
}

int UDPPort::GetPort() const
{
    return m_port;
}

std::string UDPPort::GetRemoteAddress() const
{
    return m_remoteAddress;
}

int UDPPort::GetRemotePort() const
{
    return m_remotePort;
}

bool UDPPort::IsBound() const
{
    return m_socketHandle >= 0 && m_port >= 0;
}

