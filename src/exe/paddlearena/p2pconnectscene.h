#pragma once

#include <memory>

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

        class UDPPort;
        using UDPPortPtr = std::shared_ptr<UDPPort>;

        class Protocol;
        using ProtocolPtr = std::shared_ptr<Protocol>;
    }
}

class P2PConnectScene : public sputter::game::IScene
{
public:
    P2PConnectScene(
        sputter::render::Window* pWindow,
        PaddleArena* pPaddleArena,
        const char* pClientName);

    virtual ~P2PConnectScene();

    virtual void Initialize() override;
    virtual void Uninitialize() override;

    virtual void Tick(sputter::math::FixedPoint dt) override;
    virtual void Draw() override;

private:
    void PopSceneStack();

    enum class ConnectionSceneState
    {
        Initializing = 0,
        Connecting,
        Identifying,
        Connected
    };

    static constexpr uint32_t kConnectRetryTicks = 60;
    static constexpr uint32_t kMaxConnectionRetries = 10;
    uint32_t                  m_numConnectionRetries = 0;

    static constexpr uint32_t kIdentifyRetryTicks = 60;
    static constexpr uint32_t kMaxIdentifyRetries = 10;
    uint32_t                  m_numIdentifyRetries = 0;

    bool                      m_sentIdentity = false;
    bool                      m_receivedId = false;

    ConnectionSceneState      m_state = ConnectionSceneState::Initializing;
    uint32_t                  m_numTicks = 0;

    sputter::net::UDPPortPtr  m_spPort = nullptr;
    sputter::net::ProtocolPtr m_spProtocol = nullptr;

    sputter::render::Window*  m_pWindow = nullptr;
    PaddleArena*              m_pPaddleArena = nullptr;
    const char*               m_pClientName = nullptr;
};

