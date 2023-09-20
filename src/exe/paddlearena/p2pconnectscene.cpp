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
    // TODO
}

void P2PConnectScene::Uninitialize()
{
    // TODO: No UI yet
}

void P2PConnectScene::Tick(sputter::math::FixedPoint dt)
{
    // TODO
}

void P2PConnectScene::Draw()
{
    // TODO
}
