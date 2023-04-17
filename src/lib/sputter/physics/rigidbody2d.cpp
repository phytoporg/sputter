#include "rigidbody2d.h"

#include <cstdio>

using namespace sputter;
using namespace sputter::physics;

void ToString(const RigidBody2D &rigidBody2D, char *pBuffer)
{
    char positionBuffer[128] = {};
    ToString(rigidBody2D.Position, positionBuffer);
    char velocityBuffer[128] = {};
    ToString(rigidBody2D.Velocity, velocityBuffer);
    char accelerationBuffer[128] = {};
    ToString(rigidBody2D.Acceleration, accelerationBuffer);
    char massBuffer[32] = {};
    ToString(rigidBody2D.Mass, massBuffer);

    sprintf(
        pBuffer,
        "Position: %s, Velocity: %s, Acceleration: %s, Mass: %s",
        positionBuffer,
        velocityBuffer,
        accelerationBuffer,
        massBuffer
    );
}
