#include "testobject.h"

#include <sputter/physics/rigidbodysubsystem.h>

#include <sputter/game/subsystemprovider.h>
#include <sputter/system/system.h>

using namespace sputter::game;

TestObject::TestObject(SubsystemProvider* pProvider)
    : Object(pProvider)
{
    CreateAndSetComponentByType<sputter::physics::RigidBodySubsystem>(&m_pRigidBodyComponent);
    if (!m_pRigidBodyComponent)
    {
        sputter::system::LogAndFail("Failed to find rigid body component in TestObject.");
    }
    
    // m_pRigidBodyComponent = pProvider->Get
    // Create a sprite component
}

void TestObject::Tick(float dt)
{
    // TODO: this, but also use fixed-point math
}