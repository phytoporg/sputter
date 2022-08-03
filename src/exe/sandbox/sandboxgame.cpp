#include <sputter/physics/rigidbodysubsystem.h>

#include <sputter/render/shaderstorage.h>
#include <sputter/render/texturestorage.h>
#include <sputter/render/shader.h>

#include <sputter/assets/imagedata.h>
#include <sputter/assets/textdata.h>

#include "gamestate.h"
#include "sandboxgame.h"

using namespace sputter;

static const size_t kMaxNumSprites = 10;

SandboxGame::SandboxGame(
        render::Window* pWindow,
        const std::string& assetStoragePath,
        memory::FixedMemoryAllocator allocator) 
    : m_pGameState(nullptr),
      m_pSprite(nullptr),
      m_assetStorage(assetStoragePath)
{
    physics::RigidBodySubsystemSettings settings;
    settings.MaxRigidBodies = 5;

    m_pGameState = allocator.Create<GameState>(settings);

    // Load up the ship asset
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

    // Load up the sprite shader
    auto spSpriteVertexShader = m_assetStorage.FindFirstByName("sprite_vert");
    auto pSpriteVertexShader = dynamic_cast<assets::TextData*>(spSpriteVertexShader.get());

    auto spSpriteFragmentShader = m_assetStorage.FindFirstByName("sprite_frag");
    auto pSpriteFragmentShader = dynamic_cast<assets::TextData*>(spSpriteFragmentShader.get());
    if (!m_shaderStorage.AddShader(*pSpriteVertexShader, *pSpriteFragmentShader, "sprite_shader"))
    {
        LOG(ERROR) << "Could not add sprite shader to storage.";
        return;
    }

    render::ShaderPtr spShader = m_shaderStorage.FindShaderByName("sprite_shader");
    if (!spShader)
    {
        LOG(ERROR) << "Failed to find sprite_shader in shader storage";
        return;
    }
    
    m_pSpriteSubsystem = new sputter::render::SpriteSubsystem(
        *pWindow,
        spShader.get(),
        kMaxNumSprites
        );
}

SandboxGame::~SandboxGame() {}

void SandboxGame::Tick(math::FixedPoint deltaTime)
{
    m_pGameState->RigidBodySubsystem.Tick(deltaTime);
    m_pSprite->SetPosition(m_pMainRigidBody->Position);
}

void SandboxGame::Draw()
{
    m_pSpriteSubsystem->Draw();
}

bool SandboxGame::StartGame()
{
    m_pMainRigidBody = m_pGameState->RigidBodySubsystem.CreateComponent();

    const math::FixedPoint FPTwenty(20);
    m_pMainRigidBody->Position.Set(FPTwenty, FPTwenty);

    m_pSprite = m_pSpriteSubsystem->CreateComponent();
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
