#pragma once

#include <sputter/memory/fixedmemoryallocator.h>

#include <sputter/game/game.h>

#include <sputter/physics/rigidbody2d.h>

#include <sputter/assets/assetstorage.h>

#include <sputter/render/window.h>
#include <sputter/render/spritesubsystem.h>
#include <sputter/render/shaderstorage.h>
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

    sputter::physics::RigidBody2D*    m_pMainRigidBody;

    sputter::render::ShaderStorage    m_shaderStorage;
    sputter::render::SpriteSubsystem* m_pSpriteSubsystem;
    sputter::render::Sprite*          m_pSprite;

    sputter::render::TexturePtr       m_spTexture;

    sputter::assets::AssetStorage     m_assetStorage;

    GameState*                        m_pGameState;
};

