#include "paddlearena.h"
#include "gamestate.h"
#include "gameconstants.h"

#include <sputter/physics/rigidbodysubsystem.h>

#include <sputter/render/meshsubsystem.h>
#include <sputter/render/shaderstorage.h>
#include <sputter/render/texturestorage.h>
#include <sputter/render/shader.h>

#include <sputter/math/fpvector2d.h>

#include <sputter/assets/imagedata.h>
#include <sputter/assets/textdata.h>

#include <sputter/input/inputsubsystem.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace sputter;

PaddleArena::PaddleArena(
        render::Window* pWindow,
        const std::string& assetStoragePath,
        memory::FixedMemoryAllocator allocator) 
    : m_pGameState(nullptr),
      m_assetStorage(assetStoragePath),
      m_pWindow(pWindow),
      m_storageProvider(&m_assetStorage)
{
    physics::RigidBodySubsystemSettings rigidBodySubsystemSettings;
    rigidBodySubsystemSettings.MaxRigidBodies = 5;
    m_pRigidbodySubsystem = 
        allocator.Create<sputter::physics::RigidBodySubsystem>(rigidBodySubsystemSettings);

    physics::CollisionSubsystemSettings collisionSubsystemSettings;
    m_pCollisionSubsystem = 
        allocator.Create<sputter::physics::CollisionSubsystem>(collisionSubsystemSettings);

    sputter::render::MeshSubsystemSettings meshSubsystemSettings;
    meshSubsystemSettings.MaxVertexCount = 20;
    meshSubsystemSettings.MaxMeshCount = 20;
    m_pMeshSubsystem = new sputter::render::MeshSubsystem(
        meshSubsystemSettings
    );

    sputter::input::InputSubsystemSettings inputSubsystemSettings;
    inputSubsystemSettings.pWindow = m_pWindow;

    inputSubsystemSettings.PlayerDevices[0] = sputter::input::DeviceType::KeyboardInputDevice;
    inputSubsystemSettings.PlayerDevices[1] = sputter::input::DeviceType::Invalid;

    const std::vector<sputter::input::InputMapEntry> p1InputMap = 
        { 
          { static_cast<uint32_t>(GLFW_KEY_W),   static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_UP) },
          { static_cast<uint32_t>(GLFW_KEY_S), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_DOWN) },
          { static_cast<uint32_t>(GLFW_KEY_A), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_LEFT) },
          { static_cast<uint32_t>(GLFW_KEY_D), static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_RIGHT) },
          };
    inputSubsystemSettings.pInputMapEntryArrays[0] = p1InputMap.data();
    inputSubsystemSettings.pInputMapEntryArrays[1] = nullptr;
        
    inputSubsystemSettings.pNumInputMapEntries[0] = p1InputMap.size();
    inputSubsystemSettings.pNumInputMapEntries[1] = 0;
    m_pInputSubsystem = new sputter::input::InputSubsystem(inputSubsystemSettings);

    m_subsystemProvider.AddSubsystem(m_pRigidbodySubsystem);
    m_subsystemProvider.AddSubsystem(m_pCollisionSubsystem);
    m_subsystemProvider.AddSubsystem(m_pMeshSubsystem);
    m_subsystemProvider.AddSubsystem(m_pInputSubsystem);

    m_storageProvider.AddResourceStorageByType(&m_textureStorage);
    m_storageProvider.AddResourceStorageByType(&m_shaderStorage);

    m_pGameState = allocator.Create<GameState>(&m_storageProvider, &m_subsystemProvider);
}

PaddleArena::~PaddleArena() {}

void PaddleArena::Tick(math::FixedPoint deltaTime)
{
    m_pInputSubsystem->Tick(deltaTime);
    m_pRigidbodySubsystem->Tick(deltaTime);

    m_pGameState->TheBall.Tick(deltaTime);
    m_pGameState->Arena.Tick(deltaTime);
    m_pGameState->Player1Paddle.Tick(deltaTime);
    m_pGameState->Player2Paddle.Tick(deltaTime);
}

void PaddleArena::PostTick(math::FixedPoint deltaTime)
{
    m_pCollisionSubsystem->PostTick(deltaTime);

    m_pGameState->TheBall.PostTick(deltaTime);
    m_pGameState->Player1Paddle.PostTick(deltaTime);
    m_pGameState->Player2Paddle.PostTick(deltaTime);

    // Do we need to reset the ball?
    if (m_pGameState->TheBall.IsDead())
    {
        // Ideally we wait a few frames. Handle that later!
        m_pGameState->TheBall.Reset(
            kGameConstantsBallStartPosition,
            kGameConstantsBallStartDirection
            );
    }
    
}

void PaddleArena::Draw()
{
    // TODO: This belongs in a camera?
    static const glm::mat4 OrthoMatrix =
       glm::ortho(
           0.0f, 
           static_cast<float>(m_pWindow->GetWidth()),
           static_cast<float>(m_pWindow->GetHeight()),
           0.0f,
           -1000.0f, 1000.0f);

    const glm::mat4 viewMatrix = m_pGameState->Camera.ViewMatrix4d();
    m_pMeshSubsystem->Draw(OrthoMatrix, viewMatrix);
}

bool PaddleArena::StartGame()
{
    using namespace sputter::math;

    m_pGameState->TheBall.Initialize(
        kGameConstantsBallDimensions,
        kGameConstantsBallStartPosition,
        kGameConstantsBallStartDirection);
    m_pGameState->Player1Paddle.Initialize(
        FPVector2D(20, 80),
        FPVector3D(FixedPoint(-350), FPZero, FixedPoint(-0.5)));
    m_pGameState->Player2Paddle.Initialize(
        FPVector2D(20, 80),
        FPVector3D(FixedPoint(350), FPZero, FixedPoint(-0.5)));
    m_pGameState->Arena.Initialize(FPVector2D(800, 600));        
    m_pGameState->Camera.Translate(FPVector3D(-500, 400, -200));

    return true;
}

bool PaddleArena::SaveGameState(
    void** pBuffer,
    size_t* pSize,
    size_t* pChecksum,
    int frame)
{
    return false;
}

bool PaddleArena::LoadGameState(void** pBuffer, size_t size)
{
    return false;
}

bool PaddleArena::AdvanceFrame()
{
    return false;
}
