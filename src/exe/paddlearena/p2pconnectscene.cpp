#include "p2pconnectscene.h"
#include "gamemode.h"
#include "paddlearena.h"

#include <cstring>

#include <sputter/log/log.h>
#include <sputter/net/port.h>
#include <sputter/net/protocol.h>
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
    m_state = ConnectionSceneState::Initializing;
    m_spPort.reset(new UDPPort(kClientPort));
}

void P2PConnectScene::Uninitialize()
{
    // TODO: No UI yet
}

void P2PConnectScene::Tick(sputter::math::FixedPoint dt)
{
    // TODO: retry logic for connecting
    if (m_state == ConnectionSceneState::Initializing)
    {
        m_state = ConnectionSceneState::Connecting;
        m_numConnectionRetries = 0;
    }
    else if (m_state == ConnectionSceneState::Connecting)
    {
        // TODO: Protocol should handle transport reliability
        if (m_numTicks % kConnectRetryTicks == 0 || !m_numConnectionRetries)
        {
            if (m_spPort->connect("127.0.0.1", kServerPort))
            {
                m_spProtocol.reset(new Protocol(m_spPort));
                m_state = ConnectionSceneState::Identifying;
                m_sentIdentity = false;
                m_receivedId = false;

                RELEASE_LOGLINE_INFO(LOG_NET, "Connected to server!");
            }
            else
            {
                m_numConnectionRetries++;
                if (m_numConnectionRetries >= kMaxConnectionRetries)
                {
                    RELEASE_LOGLINE_INFO(
                        LOG_NET,
                        "Connecting failed, leaving p2p scene");
                    PopSceneStack();
                }

                RELEASE_LOGLINE_INFO(
                    LOG_NET,
                    "Connection attempt failed (%d/%d)",
                    m_numConnectionRetries,
                    kMaxConnectionRetries);
            }
        }
    }
    else if (m_state == ConnectionSceneState::Identifying)
    {
        // TODO: Protocol should handle transport reliability
        if (m_numTicks % kIdentifyRetryTicks == 0 || !m_numIdentifyRetries)
        {
            // TODO: Plumb through an actual name
            if (!m_sentIdentity)
            {
                const std::string Name = "client";
                m_sentIdentity = m_spProtocol->SendHelloMessage(Name);
                if (m_sentIdentity)
                {
                    RELEASE_LOGLINE_INFO(LOG_NET, "Sent hello to server!");
                }
            }
            else if (!m_receivedId)
            {
                AssignClientIdMessage assignClientIdMessage;
                if (m_spProtocol->ReceiveAssignClientIdMessage(
                        &assignClientIdMessage))
                {
                    RELEASE_LOGLINE_INFO(
                        LOG_NET,
                        "Received client ID: %hhu",
                        assignClientIdMessage.ClientId);
                    m_receivedId = true;
                    m_pPaddleArena->SetClientId(assignClientIdMessage.ClientId);
                    m_state = ConnectionSceneState::Connected;
                }
            }
        }
    }
    else if (m_state == ConnectionSceneState::Connected)
    {
        constexpr uint32_t kSessionId = 0; // Not needed??
        ReliableUDPSession* pSession =
            new ReliableUDPSession(kSessionId, *m_spProtocol->GetUDPPort());
        m_pPaddleArena->SetProtocol(m_spProtocol);
        m_pPaddleArena->SetUDPSession(pSession);

        if (!m_spProtocol->SendClientReadyMessage(m_pPaddleArena->GetClientId()))
        {
            // TODO: Robustification 
            RELEASE_LOGLINE_ERROR(LOG_NET, "Failed to send client ready (%d/%d)");
            PopSceneStack();
        }
        else
        {
            RELEASE_LOGLINE_ERROR(LOG_NET, "Client is ready to start game.");
            m_state = ConnectionSceneState::Ready;
        }
    }
    else if (m_state == ConnectionSceneState::Ready)
    {
        StartGameMessage startGameMessage;
        if (m_spProtocol->ReceiveStartGameMessage(&startGameMessage))
        {
            RELEASE_LOGLINE_INFO(LOG_NET, "Received StartGame message, starting...");
            m_pPaddleArena->NextSceneFromP2PScreen();
        }
    }

    ++m_numTicks;
}

void P2PConnectScene::Draw()
{
    // TODO
}

void P2PConnectScene::PopSceneStack()
{
    // TODO: pop back out to the previous scene
}
