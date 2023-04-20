#include "reliableudpserver.h"
#include "reliableudpsession.h"

#include <sputter/log/log.h>
#include <sputter/core/check.h>

// LINUX++
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
// LINUX--

using namespace sputter;
using namespace sputter::net;

ReliableUDPServer::ReliableUDPServer()
{
    int socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    RELEASE_CHECK(socketFd >= 0, "Failed to create server socket");

    if (fcntl(socketFd, F_SETFL, O_NONBLOCK) < 0)
    {
        system::LogAndFail("Failed to set nonblocking flag for server socket");
    }

    RELEASE_LOGLINE_INFO(LOG_NET, "ReliableUDPServer socket created: %d", socketFd);
    m_serverSocket = socketFd;
}

ReliableUDPServer::~ReliableUDPServer()
{
    if (m_serverSocket >= 0)
    {
        ::close(m_serverSocket);
        m_serverSocket = -1;
    }

    if (m_pSession)
    {
        delete m_pSession;
        m_pSession = nullptr;
    }
}

bool ReliableUDPServer::Bind(int port)
{
    sockaddr_in bindAddress = {};
    bindAddress.sin_family = AF_INET;
    bindAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    bindAddress.sin_port = htons(port);
    if (bind(m_serverSocket, reinterpret_cast<sockaddr *>(&bindAddress), sizeof(bindAddress)) < 0)
    {
        RELEASE_LOGLINE_ERROR(LOG_NET, "Failed to bind server socket, port = %d", port);
        return false;
    }

    RELEASE_LOGLINE_INFO(LOG_NET, "ReliableUDPServer socket is bound on port %d", port);
    return true;
}

bool ReliableUDPServer::Listen()
{
    const int kMaxConnections = 1;
    if (listen(m_serverSocket, kMaxConnections) < 0)
    {
        RELEASE_LOGLINE_ERROR(LOG_NET, "ReliableUDPServer socket %d failed to listen", m_serverSocket);
        return false;
    }

    RELEASE_LOGLINE_INFO(LOG_NET, "ReliableUDPServer socket %d is listening");
    return true;
}

bool ReliableUDPServer::Accept(int *pClientIdOut)
{
    *pClientIdOut = -1;

    struct sockaddr_in inaddr;
    int inaddrSize = sizeof(inaddr);

    const int ClientFd =
        accept(
                m_serverSocket,
                reinterpret_cast<struct sockaddr*>(&inaddr),
                reinterpret_cast<socklen_t*>(&inaddrSize));
    if (ClientFd < 0)
    {
        RELEASE_LOGLINE_ERROR(
                LOG_NET,
                "ReliableUDPServer socket %d failed to accept connection",
                m_serverSocket);
        return false;
    }

    RELEASE_LOGLINE_INFO(
            LOG_NET,
            "ReliableUDPServer socket %d accepted connection for client %d",
            ClientFd);
    *pClientIdOut = ClientFd;

    RELEASE_CHECK(m_pSession == nullptr, "Creating session, but session is already active");

    // TODO: Get the address string, initialize the session
    // m_pSession = new ReliableUDPSession(ClientFd, );
    return true;
}

ssize_t ReliableUDPServer::Write(int clientId, char *pBuffer, size_t bufferSize)
{
    // TODO: validate the client ID

    if (!m_pSession)
    {
        // TODO: ERROR
        return -1;
    }

    return m_pSession->EnqueueSendData(pBuffer, bufferSize);
}

ssize_t ReliableUDPServer::Read(int clientId, char *pBuffer, size_t bufferSize)
{
    // TODO: validate the client ID

    if (!m_pSession)
    {
        // TODO: ERROR
        return -1;
    }

    return m_pSession->TryReadData(pBuffer, bufferSize);
}

void ReliableUDPServer::Close()
{
}

