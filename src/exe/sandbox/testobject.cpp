#include "testobject.h"

#include <sputter/physics/rigidbodysubsystem.h>

#include <sputter/render/spritesubsystem.h>

#include <sputter/game/subsystemprovider.h>
#include <sputter/system/system.h>

using namespace sputter::game;

TestObject::TestObject(SubsystemProvider* pProvider)
    : Object(pProvider)
{
    CreateAndSetComponentByType<sputter::physics::RigidBodySubsystem>(&m_pRigidBodyComponent);
    if (!m_pRigidBodyComponent)
    {
        sputter::system::LogAndFail("Failed to create rigid body component in TestObject.");
    }
    
    CreateAndSetComponentByType<sputter::render::SpriteSubsystem>(&m_pSpriteComponent);
    if (!m_pSpriteComponent)
    {
        sputter::system::LogAndFail("Failed to create sprite component in TestObject.");
    }
}

void TestObject::Tick(sputter::math::FixedPoint /*unreferenced*/)
{
    m_pSpriteComponent->SetPosition(m_pRigidBodyComponent->Position);
}

void TestObject::Initialize(
    const sputter::math::FPVector2D& initialPosition,
    sputter::render::TexturePtr spTexture
    )
{
    m_pRigidBodyComponent->Position.Set(initialPosition);
    m_pSpriteComponent->SetPosition(m_pRigidBodyComponent->Position);
    m_pSpriteComponent->SetTexturePtr(spTexture);
    m_pSpriteComponent->SetDimensions(100.0f, 100.0f);
}