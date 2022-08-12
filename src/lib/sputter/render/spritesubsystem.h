#pragma once

#include <sputter/core/subsystem.h>
// TODO: This should probably be in 'core'?
#include <sputter/game/subsystemtype.h>
#include "spritebatch.h"
#include "sprite.h"
#include "mesh.h"
#include "uniform.h"

#include <string>

namespace sputter { namespace assets {
    class AssetStorage;
}}

namespace sputter { namespace render {
    class Shader;
    class ShaderStorage;

    struct SpriteSubsystemSettings
    {
        SpriteSubsystemSettings() 
            :  SpriteVertexShaderName("sprite_vert"),
               SpriteFragmentShaderName("sprite_frag"),
               MaxSpriteCount(256)
        {}

        std::string SpriteVertexShaderName;
        std::string SpriteFragmentShaderName;
        size_t      MaxSpriteCount;
    };

    class SpriteSubsystem : public core::ISubsystem<Sprite>
    {
    public:
        static const game::SubsystemType SubsystemId =
            game::SubsystemType::TYPE_SPRITE;

        SpriteSubsystem(
            sputter::assets::AssetStorage* pAssetStorage,
            ShaderStorage* pShaderStorage,
            const SpriteSubsystemSettings& settings
            );

        virtual void Tick(math::FixedPoint dt) override;

        virtual Sprite* CreateComponent(const Sprite::InitializationParameters& params) override;
        virtual void ReleaseComponent(Sprite* pSprite) override;

        void Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix);

    private:
        SpriteSubsystem() = delete;
        SpriteSubsystem(const SpriteSubsystem& other) = delete;

        size_t              m_spriteCount;
        size_t              m_maxSpriteCount;

        std::vector<Sprite> m_sprites;
        std::vector<bool>   m_spritesValidArray;

        Shader*             m_pSpriteShader;

        // TODO(philjo 2/7/2021): support for multiple batches-- can start with
        // hardcoding according to basic texture needs, then maybe an LRU for
        // batch resources?
        SpriteBatch         m_spriteBatch;
    };
}}
