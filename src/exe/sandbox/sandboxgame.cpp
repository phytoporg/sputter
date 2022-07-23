#include <sputter/physics/rigidbodysubsystem.h>
#include "gamestate.h"
#include "sandboxgame.h"

using namespace sputter;

static const size_t kMaxNumSprites = 10;

SandboxGame::SandboxGame(
        render::Window* pWindow,
        render::TexturePtr spTexture,
        memory::FixedMemoryAllocator allocator) 
    : m_pGameState(nullptr),
      m_spriteSubsystem(*pWindow, kMaxNumSprites),
      m_pSprite(nullptr),
      m_spTexture(spTexture)
{
    physics::RigidBodySubsystemSettings settings;
    settings.MaxRigidBodies = 5;

    m_pGameState = allocator.Create<GameState>(settings);
}

SandboxGame::~SandboxGame() {}

void SandboxGame::Tick(math::FixedPoint deltaTime)
{
    m_pGameState->RigidBodySubsystem.Tick(deltaTime);
    m_pSprite->SetPosition(m_pMainRigidBody->Position);
}

void SandboxGame::Draw()
{
    m_spriteSubsystem.Draw();
}

bool SandboxGame::StartGame()
{
    m_pMainRigidBody = m_pGameState->RigidBodySubsystem.CreateComponent();

    const math::FixedPoint FPTwenty(20);
    m_pMainRigidBody->Position.Set(FPTwenty, FPTwenty);

    m_pSprite = m_spriteSubsystem.CreateComponent();
    m_pSprite->SetPosition(m_pMainRigidBody->Position);
    m_pSprite->SetTexturePtr(m_spTexture);
    m_pSprite->SetDimensions(100.0f, 100.0f);

    return true;
}

bool SandboxGame::SaveGameState(
    void** pBuffer,
    size_t* pSize,
    size_t* pChecksum,
    int frame)
{
    return false;
}

bool SandboxGame::LoadGameState(void** pBuffer, size_t size)
{
    return false;
}

bool SandboxGame::AdvanceFrame()
{
    return false;
}
