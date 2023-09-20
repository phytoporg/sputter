#pragma once

#include <sputter/game/scene.h>
#include <sputter/net/port.h>

class PaddleArena;

// Forward declarations
namespace sputter
{
    namespace render
    {
        class Window;
    }

    namespace net
    {
        class ReliableUDPSession;
    }
}

class P2PConnectScene : public sputter::game::IScene
{
public:
    P2PConnectScene(
        sputter::render::Window* pWindow,
        PaddleArena* pPaddleArena);

    virtual ~P2PConnectScene();

    virtual void Initialize() override;
    virtual void Uninitialize() override;

    virtual void Tick(sputter::math::FixedPoint dt) override;
    virtual void Draw() override;

private:
    sputter::net::ReliableUDPSession*         m_pUdpSession = nullptr;
    sputter::net::UDPPort                     m_port;

    sputter::render::Window*                  m_pWindow = nullptr;
    PaddleArena*                              m_pPaddleArena = nullptr;
};
