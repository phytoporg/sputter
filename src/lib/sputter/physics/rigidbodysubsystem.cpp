#include "rigidbodysubsystem.h"

#include <sputter/system/system.h>

#include <string>
#include <cassert>

namespace sputter { namespace physics {
    RigidBodySubsystem::RigidBodySubsystem(const RigidBodySubsystemSettings& settings)
        : m_isGravityEnabled(settings.IsGravityEnabled),
          m_gravity(settings.Gravity)
    {
        memset(m_rigidBodies, 0, sizeof(m_rigidBodies));
        memset(m_validRigidBodyVector, 0, sizeof(m_validRigidBodyVector));
    }

    void RigidBodySubsystem::Tick(math::FixedPoint dt)
    {
        // TODO(philjo 2/7/2021):
        // Simplifying the stable and self-starting terms for verlet integration
        // from:
        // http://www.physics.udel.edu/~bnikolic/teaching/phys660/numerical_ode/node5.html
        //
        // This is gonna have to be adjusted when arbitrary forces can be 
        // applied during simulation. Right now, assuming constant acceleration
        // (which is the simplification, i.e. a_n+1 = a_n always).

        for (size_t i = 0; i < m_rigidBodyCount; ++i)
        {
            if (!m_validRigidBodyVector[i]) { continue; }

            RigidBody2D& rigidBody = m_rigidBodies[i];
            
            const math::FPVector2D xn = rigidBody.Position;
            const math::FPVector2D vn = rigidBody.Velocity;
            math::FPVector2D an = rigidBody.Acceleration;

            if (m_isGravityEnabled)
            {
                an += m_gravity;
            }

            rigidBody.Position = xn + vn * dt + an * dt * dt * math::FPOneHalf;
            rigidBody.Velocity = vn + an * dt;
        }
    }

    RigidBody2D* RigidBodySubsystem::CreateComponent(const RigidBody2D::InitializationParameters& params)
    {
        // NOTE(philjo 2/7/2021): We're passing around raw pointers to these
        // components and assuming they're all stored in m_rigidBodies, so
        // there can't be *any* reallocs allowed. Sputter's for small games, so
        // ideally we don't run into issues here but this is why we call out the
        // max number of rigidbodies when initializing the subsystem; that's
        // a pattern which will likely continue across most of Sputter, where
        // it makes sense anyway.
        if (m_rigidBodyCount >= kMaxRigidBodies)
        {
            system::LogAndFail(
                "Reached the max number of rigidbodies: " +
                std::to_string(kMaxRigidBodies));
        }

        const size_t BadIndex = static_cast<size_t>(-1);
        size_t nextIndex = BadIndex;
        for (size_t i = 0; i < m_rigidBodyCount; ++i)
        {
            if (!m_validRigidBodyVector[i])
            {
                nextIndex = i;
                break;
            }
        }

        // All paths from here lead to adding a new rigidbody, so increment now.
        if (nextIndex == BadIndex)
        {
            m_rigidBodies[m_rigidBodyCount] = RigidBody2D();
            m_validRigidBodyVector[m_rigidBodyCount] = true;
            return &m_rigidBodies[m_rigidBodyCount++];
        }
        else
        {
            ++m_rigidBodyCount;
            m_validRigidBodyVector[nextIndex] = true;
            return &m_rigidBodies[nextIndex];
        }
    }

    void RigidBodySubsystem::ReleaseComponent(RigidBody2D* pComponent)
    {
        // This pointer really should be in the array. Make sure that's the
        // case.
        const size_t Index = static_cast<size_t>(pComponent - &m_rigidBodies[0]);
        if (Index >= m_rigidBodyCount)
        {
            system::LogAndFail(
                "RigidBodySubsystem::ReleaseComponent: bad pointer arg");
        }

        m_validRigidBodyVector[Index] = false;
        m_rigidBodyCount--;
    }

    bool RigidBodySubsystem::Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes)
    {
        size_t bytesWritten = 0;
        WRITE(m_rigidBodies, pBuffer, bytesWritten, maxBytes);
        bytesWritten += sizeof(m_rigidBodies);

        WRITE(m_validRigidBodyVector, pBuffer, bytesWritten, maxBytes);
        bytesWritten += sizeof(m_validRigidBodyVector);

        WRITE(m_isGravityEnabled, pBuffer, bytesWritten, maxBytes);
        bytesWritten += sizeof(m_isGravityEnabled);

        WRITE(m_gravity, pBuffer, bytesWritten, maxBytes);
        bytesWritten += sizeof(m_gravity);

        WRITE(m_rigidBodyCount, pBuffer, bytesWritten, maxBytes);
        bytesWritten += sizeof(m_rigidBodyCount);

        *pBytesWrittenOut += bytesWritten;
        return true;
    }

    bool RigidBodySubsystem::Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes)
    {
        size_t bytesRead = 0;
        WRITE(m_rigidBodies, pBuffer, bytesRead, maxBytes);
        bytesRead += sizeof(m_rigidBodies);

        WRITE(m_validRigidBodyVector, pBuffer, bytesRead, maxBytes);
        bytesRead += sizeof(m_validRigidBodyVector);

        WRITE(m_isGravityEnabled, pBuffer, bytesRead, maxBytes);
        bytesRead += sizeof(m_isGravityEnabled);

        WRITE(m_gravity, pBuffer, bytesRead, maxBytes);
        bytesRead += sizeof(m_gravity);

        WRITE(m_rigidBodyCount, pBuffer, bytesRead, maxBytes);
        bytesRead += sizeof(m_rigidBodyCount);

        *pBytesReadOut += bytesRead;
        return true;
    }
}}
