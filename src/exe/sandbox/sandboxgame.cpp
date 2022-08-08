#include <sputter/physics/rigidbodysubsystem.h>

#include <sputter/render/meshsubsystem.h>
#include <sputter/render/shaderstorage.h>
#include <sputter/render/texturestorage.h>
#include <sputter/render/shader.h>

#include <sputter/math/fpvector2d.h>

#include <sputter/assets/imagedata.h>
#include <sputter/assets/textdata.h>

#include "gamestate.h"
#include "sandboxgame.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace sputter;

static const size_t kMaxNumSprites = 10;

SandboxGame::SandboxGame(
        render::Window* pWindow,
        const std::string& assetStoragePath,
        memory::FixedMemoryAllocator allocator) 
    : m_pGameState(nullptr),
      m_pSprite(nullptr),
      m_assetStorage(assetStoragePath),
      m_pWindow(pWindow),
      m_storageProvider(&m_assetStorage)
{
    physics::RigidBodySubsystemSettings rigidBodySubsystemSettings;
    rigidBodySubsystemSettings.MaxRigidBodies = 5;
    m_pRigidbodySubsystem = 
        allocator.Create<sputter::physics::RigidBodySubsystem>(rigidBodySubsystemSettings);

    sputter::render::SpriteSubsystemSettings spriteSubsystemSettings;
    m_pSpriteSubsystem = new sputter::render::SpriteSubsystem(
        &m_assetStorage,
        &m_shaderStorage,
        spriteSubsystemSettings);

    sputter::render::MeshSubsystemSettings meshSubsystemSettings;
    meshSubsystemSettings.MaxVertexCount = 8;
    m_pMeshSubsystem = new sputter::render::MeshSubsystem(
        meshSubsystemSettings
    );

    m_subsystemProvider.AddSubsystem(m_pRigidbodySubsystem);
    m_subsystemProvider.AddSubsystem(m_pSpriteSubsystem);
    m_subsystemProvider.AddSubsystem(m_pMeshSubsystem);

    m_storageProvider.AddResourceStorageByType(&m_textureStorage);
    m_storageProvider.AddResourceStorageByType(&m_shaderStorage);

    m_pGameState = allocator.Create<GameState>(&m_storageProvider, &m_subsystemProvider);
}

SandboxGame::~SandboxGame() {}

void SandboxGame::Tick(math::FixedPoint deltaTime)
{
    m_pRigidbodySubsystem->Tick(deltaTime);
    m_pGameState->MainShip.Tick(deltaTime);
    m_pGameState->MainCube.Tick(deltaTime);
}

void SandboxGame::Draw()
{
    // TODO: This belongs in a camera?
    static const glm::mat4 OrthoMatrix =
       glm::ortho(
           0.0f, 
           static_cast<float>(m_pWindow->GetWidth()),
           static_cast<float>(m_pWindow->GetHeight()),
           0.0f,
           -1.0f, 1.0f);

    m_pSpriteSubsystem->Draw(OrthoMatrix);
    m_pMeshSubsystem->Draw(OrthoMatrix);
}

bool SandboxGame::StartGame()
{
    using namespace sputter::math;
    const FPVector2D ShipStartPosition(FPTwenty, FPTwenty);
    m_pGameState->MainShip.Initialize(ShipStartPosition);

    const FixedPoint CubeSize(100);
    const FPVector3D CubeStartPosition(FPThirty, FPThirty, FPZero);
    m_pGameState->MainCube.Initialize(
        CubeSize,
        CubeStartPosition
        );

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
