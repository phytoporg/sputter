#include "p2pconnectscene.h"
#include "gamemode.h"
#include "paddlearena.h"
#include "protocol.h"

#include <sputter/log/log.h>
#include <sputter/net/port.h>
#include <sputter/net/reliableudpsession.h>

static constexpr int kClientPort = 5001;
static constexpr int kServerPort = 5002;

using namespace sputter;
using namespace sputter::net;

P2PConnectScene::P2PConnectScene(sputter::render::Window *pWindow, PaddleArena *pPaddleArena)
    :  m_port(pPaddleArena->GetGameMode() == GameMode::Server ? kServerPort : kClientPort),
       m_pWindow(pWindow),
       m_pPaddleArena(pPaddleArena)
{}

P2PConnectScene::~P2PConnectScene()
{}

void P2PConnectScene::Initialize()
{
    // TODO: No UI yet
    const GameMode GameMode = m_pPaddleArena->GetGameMode();
    if(GameMode == GameMode::Server)
    {
        m_serverConnectState = ServerConnectState::Starting;
    }
    else if(GameMode == GameMode::Client)
    {
        m_clientConnectState = ClientConnectState::Starting;
    }
}

void P2PConnectScene::Uninitialize()
{
    // TODO: No UI yet
}

void P2PConnectScene::Tick(sputter::math::FixedPoint dt)
{
    // TODO: This is all way hacky, make it better and robust
    // SoMe DaYyY
    const GameMode GameMode = m_pPaddleArena->GetGameMode();
    if (GameMode == GameMode::Client)
    {
        if (TickClient())
        {
            m_pPaddleArena->SetUDPSession(m_pUdpSession);
            m_pPaddleArena->NextSceneFromP2PScreen();
        }
    }
    else if (GameMode == GameMode::Server)
    {
        if (TickServer())
        {
            m_pPaddleArena->SetUDPSession(m_pUdpSession);
            m_pPaddleArena->NextSceneFromP2PScreen();
        }
    }
    else
    {
        system::LogAndFail("Unexpected game mode in P2PConnection scene");
    }

    if (m_pUdpSession)
    {
        m_pUdpSession->Tick();
    }
}

void P2PConnectScene::Draw()
{}

bool P2PConnectScene::TickClient()
{
    if (m_clientConnectState == ClientConnectState::Starting)
    {
        if (!m_port.bind())
        {
            RELEASE_LOGLINE_ERROR(LOG_NET, "Client failed to bind to local port");
            return false;
        }

        if (!m_port.connect(
            m_pPaddleArena->GetRemoteServerAddress(),
            m_pPaddleArena->GetRemoteServerPort()))
        {
            RELEASE_LOGLINE_ERROR(LOG_NET, "Client failed to connect to remote server");
            return false;
        }

        RELEASE_LOGLINE_INFO(LOG_NET, "P2P client started");
        m_clientConnectState = ClientConnectState::SendingHello;
        return false;
    }
    else if (m_clientConnectState == ClientConnectState::SendingHello)
    {
        // TODO: Retry logic
        static MessageHeader s_hello =
                { .Type = MessageType::Hello, .MessageSize = sizeof(HelloMessage) };
        const int Sent =
            m_port.send(
                reinterpret_cast<char*>(&s_hello),
                sizeof(s_hello),
                m_port.GetRemoteAddress(),
                m_port.GetRemotePort());
        if (Sent != sizeof(s_hello))
        {
            RELEASE_LOGLINE_ERROR(LOG_NET, "Client failed to send hello message");
            return false; // TODO: Fail vector, this'll just tick again
        }

        RELEASE_LOGLINE_INFO(LOG_NET, "Client sent hello");
        m_clientConnectState = ClientConnectState::AwaitingServerResponse;
        return false;
    }
    else if (m_clientConnectState == ClientConnectState::AwaitingServerResponse)
    {
        if (!m_pUdpSession)
        {
            m_pUdpSession =
                new ReliableUDPSession(
                    0,
                    m_port,
                    m_pPaddleArena->GetRemoteServerAddress(),
                    m_pPaddleArena->GetRemoteServerPort());

            if (!m_pUdpSession)
            {
                RELEASE_LOGLINE_ERROR(LOG_NET, "Client failed to create UDP session");
                return false;
            }
        }

        HelloMessage hello;
        std::string address;
        const int Received = m_pUdpSession->TryReadData(
            reinterpret_cast<char *>(&hello),
            sizeof(hello));
        if (!Received)
        {
            RELEASE_LOGLINE_VERYVERBOSE(LOG_NET, "Nothing receives in client tick this frame");
            return false;
        }

        if (Received != sizeof(hello))
        {
            RELEASE_LOGLINE_ERROR(LOG_NET, "Received unexpected packet size");
            return false;
        }

        if (hello.Header.Type != MessageType::Hello || hello.Header.MessageSize != sizeof(hello))
        {
            RELEASE_LOGLINE_ERROR(
                LOG_NET,
                "Malformed hello message: Type = %d, Size = %u (%u)",
                hello.Header.Type,
                hello.Header.MessageSize,
                sizeof(hello));
            return false;
        }

        RELEASE_LOGLINE_INFO(LOG_NET, "Client received server hello");
        m_clientConnectState = ClientConnectState::Connected;

        return true;
    }
    else if (m_clientConnectState == ClientConnectState::Connected)
    {
        return true;
    }

    // Shouldn't get here
    RELEASE_LOGLINE_WARNING(LOG_NET, "Unexpected code path in client tick");
    return false;
}

bool P2PConnectScene::TickServer()
{
    if (m_serverConnectState == ServerConnectState::Starting)
    {
        if (!m_port.bind())
        {
            RELEASE_LOGLINE_ERROR(LOG_NET, "Server port failed to bind");
            return false;
        }

        RELEASE_LOGLINE_INFO(LOG_NET, "P2P server has started");
        m_serverConnectState = ServerConnectState::AwaitingClientHello;
        return false;
    }
    else if (m_serverConnectState == ServerConnectState::AwaitingClientHello)
    {
        HelloMessage hello;
        std::string address;
        const int Received = m_port.receive(&hello, sizeof(hello), &address);
        if (!Received)
        {
            RELEASE_LOGLINE_VERYVERBOSE(LOG_NET, "Nothing receives in server tick this frame");
            return false;
        }

        if (Received != sizeof(hello))
        {
            RELEASE_LOGLINE_ERROR(LOG_NET, "Received unexpected packet size");
            return false;
        }

        if (hello.Header.Type != MessageType::Hello || hello.Header.MessageSize != sizeof(hello))
        {
            RELEASE_LOGLINE_ERROR(
                LOG_NET,
                "Malformed hello message: Type = %d, Size = %u (%u)",
                hello.Header.Type,
                hello.Header.MessageSize,
                sizeof(hello));
            return false;
        }

        RELEASE_LOGLINE_INFO(
                LOG_NET,
                "Server received hello message from %s",
                address.c_str());

        m_pUdpSession =
            new ReliableUDPSession(
                0,
                m_port,
                address,
                kClientPort);
        if (!m_pUdpSession)
        {
            RELEASE_LOGLINE_ERROR(LOG_NET, "Server failed to create UDP session");
            return false;
        }

        RELEASE_LOGLINE_INFO(LOG_NET, "Server established session");
        m_serverConnectState = ServerConnectState::SendingHello;
        return false;
    }
    else if (m_serverConnectState == ServerConnectState::SendingHello)
    {
        static MessageHeader s_hello =
            { .Type = MessageType::Hello, .MessageSize = sizeof(HelloMessage) };
        const int Sent = m_pUdpSession->EnqueueSendData(
                reinterpret_cast<char*>(&s_hello),
                sizeof(s_hello));
        if (Sent != sizeof(s_hello))
        {
            RELEASE_LOGLINE_ERROR(LOG_NET, "Server failed to send hello message");
            return false; // TODO: Fail vector, this'll just tick again
        }

        RELEASE_LOGLINE_INFO(LOG_NET, "Server sent hello and is now connected");
        m_serverConnectState = ServerConnectState::Connected;
        return true;
    }
    else if (m_serverConnectState == ServerConnectState::Connected)
    {
        return true;
    }

    // Shouldn't get here
    RELEASE_LOGLINE_WARNING(
        LOG_NET,
        "Unexpected code path in server tick, state = %d");
    return false;
}
