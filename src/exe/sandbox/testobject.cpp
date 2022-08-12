#include "testobject.h"

#include <sputter/assets/assetstorage.h>
#include <sputter/assets/assetstorageprovider.h>

#include <sputter/physics/rigidbodysubsystem.h>

#include <sputter/render/spritesubsystem.h>
#include <sputter/render/texturestorage.h>

#include <sputter/game/subsystemprovider.h>
#include <sputter/system/system.h>

using namespace sputter::game;

const std::string TestObject::kShipImageAssetName = "ship";
const std::string TestObject::kShipTextureName = "ship_texture";

TestObject::TestObject(
    sputter::assets::AssetStorageProvider* pStorageProvider,
    SubsystemProvider* pSubsystemProvider)
    : Object(pStorageProvider, pSubsystemProvider)
{
    {
        sputter::physics::RigidBody2D::InitializationParameters params = {};
        CreateAndSetComponentByType<sputter::physics::RigidBodySubsystem>(&m_pRigidBodyComponent, params);
        if (!m_pRigidBodyComponent)
        {
            sputter::system::LogAndFail("Failed to create rigid body component in TestObject.");
        }
    }
    
    {
        sputter::render::Sprite::InitializationParameters params = {};
        CreateAndSetComponentByType<sputter::render::SpriteSubsystem>(&m_pSpriteComponent, params);
        if (!m_pSpriteComponent)
        {
            sputter::system::LogAndFail("Failed to create sprite component in TestObject.");
        }
    }

    sputter::assets::AssetStorage* pStorage = m_pAssetStorageProvider->GetGeneralStorage();
    auto pTextureStorage = m_pAssetStorageProvider->GetStorageByType<sputter::render::TextureStorage>();
    if (!pTextureStorage->AddTextureFromImageAssetName(pStorage, kShipImageAssetName, kShipTextureName))
    {
        sputter::system::LogAndFail("Failed to add test object texture!");
    }
}

void TestObject::Tick(sputter::math::FixedPoint /*unreferenced*/)
{
    m_pSpriteComponent->SetPosition(m_pRigidBodyComponent->Position);
}

void TestObject::Initialize(
    const sputter::math::FPVector2D& initialPosition
    )
{
    m_pRigidBodyComponent->Position.Set(initialPosition);
    m_pSpriteComponent->SetPosition(m_pRigidBodyComponent->Position);
    m_pSpriteComponent->SetDimensions(100.0f, 100.0f);

    auto pTextureStorage = m_pAssetStorageProvider->GetStorageByType<sputter::render::TextureStorage>();
    if (!pTextureStorage)
    {
        sputter::system::LogAndFail("Could not retrieve texture storage.");
    }
    
    sputter::render::TexturePtr spShipTexture = pTextureStorage->FindTextureByName(kShipTextureName);
    if (!spShipTexture)
    {
        sputter::system::LogAndFail("Failed to find ship texture after addding it!");
    }

    m_pSpriteComponent->SetTexturePtr(spShipTexture);
}