#include "p2pconnectscene.h"
#include "gamemode.h"
#include "paddlearena.h"

#include <cstring>

#include <sputter/log/log.h>
#include <sputter/net/port.h>
#include <sputter/net/messageprotocol.h>
#include <sputter/net/reliableudpsession.h>

static constexpr int kClientPort = 7000;
static constexpr int kServerPort = 7001;

using namespace sputter;
using namespace sputter::net;

P2PConnectScene::P2PConnectScene(
    sputter::render::Window *pWindow,
    PaddleArena *pPaddleArena,
    const char* pClientName)
    :  m_pWindow(pWindow),
       m_pPaddleArena(pPaddleArena),
       m_pClientName(pClientName)
{
    const GameMode GameMode = pPaddleArena->GetGameMode();
    // TODO:
    // If server, spawn server process
    // If client, attempt to connect to the server until timing out
    //
    // For now, always be a client and we'll manually launch the server locally to keep
    // things simple.
}

P2PConnectScene::~P2PConnectScene()
{}

void P2PConnectScene::Initialize()
{
    m_spPort.reset(new UDPPort(kClientPort));

    // TODO: parameterize this, send "hello" in tick once we have a robust transport
    if (!m_spPort->connect("127.0.0.1", kServerPort))
    {
        RELEASE_LOGLINE_ERROR(LOG_NET, "Failed to connect to localhost server");
    }

    // Send a hello now
    HelloMessage helloMessage;
    if (!CreateHelloMessage("client", strlen("client"), helloMessage))
    {
        RELEASE_LOGLINE_ERROR(
            LOG_NET,
            "Failed to create hello message to send to new client");
        return;
    }

    const size_t ExpectedSize = 
        HelloMessage::GetExpectedSize("client", sizeof("client"));
    const int sent = m_spPort->send(&helloMessage, ExpectedSize);
    if (sent != ExpectedSize)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_NET,
            "Failed to send Hello to server. Sent %u, not %u",
            sent, ExpectedSize);
        return;
    }
}

void P2PConnectScene::Uninitialize()
{
    // TODO: No UI yet
}

void P2PConnectScene::Tick(sputter::math::FixedPoint dt)
{
    // TODO: retry logic for connecting
    MessageHeader header;
    const int NumReceived = m_spPort->receive(&header, sizeof(header));
    if (NumReceived > 0)
    {
        if (header.Type == MessageType::Hello)
        {
            RELEASE_LOGLINE_INFO(LOG_NET, "Got hello from server!");
        }
    }
}

void P2PConnectScene::Draw()
{
    // TODO
}
