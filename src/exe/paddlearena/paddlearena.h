#pragma once

#include <sputter/memory/fixedmemoryallocator.h>

#include <sputter/game/game.h>
#include <sputter/game/subsystemprovider.h>

#include <sputter/physics/rigidbodysubsystem.h>
#include <sputter/physics/rigidbody2d.h>

#include <sputter/physics/collisionsubsystem.h>
#include <sputter/physics/collision.h>

#include <sputter/assets/assetstorage.h>
#include <sputter/assets/assetstorageprovider.h>

#include <sputter/render/window.h>
#include <sputter/render/spritesubsystem.h>
#include <sputter/render/shaderstorage.h>
#include <sputter/render/texturestorage.h>
#include <sputter/render/texture.h>

#include <sputter/input/inputsubsystem.h>

#include <sputter/math/fixedpoint.h>

#include <string>

#include "gamestate.h"

enum class PaddleArenaInput {
    // Starting out with forreal pong
    INPUT_MOVE_UP,
    INPUT_MOVE_DOWN,
    INPUT_MOVE_LEFT,
    INPUT_MOVE_RIGHT
};

class PaddleArena : public sputter::game::Game
{
public:
    PaddleArena(
        sputter::render::Window* pWindow,
        const std::string& assetStoragePath,
        sputter::memory::FixedMemoryAllocator allocator);
    virtual ~PaddleArena();

    virtual void Tick(sputter::math::FixedPoint deltaTime) override;
    virtual void Draw() override;

    virtual bool StartGame() override;
    virtual bool SaveGameState(
        void** pBuffer,
        size_t* pSize,
        size_t* pChecksum,
        int frame) override;
    virtual bool LoadGameState(void** pBuffer, size_t size) override;
    virtual bool AdvanceFrame() override;

private:
    PaddleArena() = delete;
    PaddleArena(const PaddleArena& other) = delete;

    sputter::physics::RigidBodySubsystem* m_pRigidbodySubsystem;
    sputter::physics::CollisionSubsystem* m_pCollisionSubsystem;

    sputter::render::ShaderStorage        m_shaderStorage;
    sputter::render::MeshSubsystem*       m_pMeshSubsystem;
    sputter::render::Window*              m_pWindow;

    sputter::input::InputSubsystem*       m_pInputSubsystem;

    sputter::assets::AssetStorageProvider m_storageProvider;
    sputter::game::SubsystemProvider      m_subsystemProvider;

    sputter::render::TextureStorage       m_textureStorage;

    sputter::assets::AssetStorage         m_assetStorage;

    GameState*                            m_pGameState;
};
