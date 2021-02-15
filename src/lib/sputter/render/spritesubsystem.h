#pragma once

#include <sputter/core/subsystem.h>
#include "spriteshader.h"
#include "spritebatch.h"
#include "sprite.h"
#include "window.h"

namespace sputter { namespace render {
    class SpriteSubsystem : core::ISubsystem<Sprite>
    {
    public:
        // TODO: settings?
        SpriteSubsystem(Window& window, size_t maxSpriteCount);

        virtual void Tick(math::FixedPoint dt) override;

        virtual Sprite* CreateComponent() override;
        virtual void ReleaseComponent(Sprite* pSprite) override;

        void Draw();

    private:
        SpriteSubsystem() = delete;
        SpriteSubsystem(const SpriteSubsystem& other) = delete;

        Window&             m_window;

        size_t              m_spriteCount;;
        size_t              m_maxSpriteCount;

        std::vector<Sprite> m_sprites;
        std::vector<bool>   m_spritesValidArray;

        SpriteShader        m_spriteShader;

        // TODO(philjo 2/7/2021): support for multiple batches-- can start with
        // hardcoding according to basic texture needs, then maybe an LRU for
        // batch resources?
        SpriteBatch         m_spriteBatch;
    };
}}
