#include <sputter/physics/rigidbodysubsystem.h>

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
      m_pWindow(pWindow)
{
    const std::string ShipAssetName = "ship";
    auto spImageAsset = m_assetStorage.FindFirstByName(ShipAssetName);
    if (!spImageAsset)
    {
        LOG(ERROR) << "Could not find ship texture asset" << std::endl;
        return;
    }

    if (spImageAsset->GetType() != assets::AssetDataType::IMAGE_ASSET)
    {
        LOG(ERROR) << "Asset is not the correct type" << std::endl;
        return;
    }

    auto pImageData = dynamic_cast<assets::ImageData*>(spImageAsset.get());
    if (!pImageData->pBytes)
    {
        LOG(ERROR) << "No bytes!" << std::endl;
        return;
    }

    render::TextureStorage textureStorage;
    if (!textureStorage.AddTexture(*pImageData, ShipAssetName))
    {
        LOG(ERROR) << "Failed to add texture.";
        return;
    }

    if (!(m_spTexture = textureStorage.FindTextureByName(ShipAssetName)))
    {
        LOG(ERROR) << "Couldnt find the texture we just added.";
        return;
    }

    physics::RigidBodySubsystemSettings rigidBodySubsystemSettings;
    rigidBodySubsystemSettings.MaxRigidBodies = 5;
    m_pRigidbodySubsystem = allocator.Create<sputter::physics::RigidBodySubsystem>(rigidBodySubsystemSettings);

    sputter::render::SpriteSubsystemSettings spriteSubsystemSettings;
    m_pSpriteSubsystem = new sputter::render::SpriteSubsystem(
        &m_assetStorage,
        &m_shaderStorage,
        spriteSubsystemSettings);

    m_subsystemProvider.AddSubsystem(m_pRigidbodySubsystem);
    m_subsystemProvider.AddSubsystem(m_pSpriteSubsystem);

    m_pGameState = allocator.Create<GameState>(&m_subsystemProvider);
}

SandboxGame::~SandboxGame() {}

void SandboxGame::Tick(math::FixedPoint deltaTime)
{
    m_pRigidbodySubsystem->Tick(deltaTime);
    m_pGameState->MainShip.Tick(deltaTime);
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
}

bool SandboxGame::StartGame()
{
    const sputter::math::FixedPoint FPTwenty(20);
    const sputter::math::FPVector2D ShipStartPosition(FPTwenty, FPTwenty);
    m_pGameState->MainShip.Initialize(ShipStartPosition, m_spTexture);

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
