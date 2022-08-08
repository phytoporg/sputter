#pragma once

#include <sputter/memory/fixedmemoryallocator.h>

#include <sputter/game/game.h>
#include <sputter/game/subsystemprovider.h>

#include <sputter/physics/rigidbodysubsystem.h>
#include <sputter/physics/rigidbody2d.h>

#include <sputter/assets/assetstorage.h>
#include <sputter/assets/assetstorageprovider.h>

#include <sputter/render/window.h>
#include <sputter/render/spritesubsystem.h>
#include <sputter/render/shaderstorage.h>
#include <sputter/render/texturestorage.h>
#include <sputter/render/texture.h>

#include <sputter/math/fixedpoint.h>

#include <string>

#include "gamestate.h"

class SandboxGame : public sputter::game::Game
{
public:
    SandboxGame(
        sputter::render::Window* pWindow,
        const std::string& assetStoragePath,
        sputter::memory::FixedMemoryAllocator allocator);
    virtual ~SandboxGame();

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
    SandboxGame() = delete;
    SandboxGame(const SandboxGame& other) = delete;

    sputter::physics::RigidBodySubsystem* m_pRigidbodySubsystem;

    sputter::render::ShaderStorage        m_shaderStorage;
    sputter::render::SpriteSubsystem*     m_pSpriteSubsystem;
    sputter::render::MeshSubsystem*       m_pMeshSubsystem;
    sputter::render::Sprite*              m_pSprite;
    sputter::render::Window*              m_pWindow;

    sputter::assets::AssetStorageProvider m_storageProvider;
    sputter::game::SubsystemProvider      m_subsystemProvider;

    sputter::render::TextureStorage       m_textureStorage;

    sputter::assets::AssetStorage         m_assetStorage;

    GameState*                            m_pGameState;
};

