#include "spritesubsystem.h"

#include <sputter/system/system.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// TODO(philjo 2/7/2021): Most of this implementation is ripped straight from
// the rigidbody subsystem. Subsystem component lifetime management might 
// benefit from generalization?? We'll see after we implement a few more.
namespace sputter { namespace render {
    SpriteSubsystem::SpriteSubsystem(Window& window, size_t maxSpriteCount)
        : m_window(window),
          m_maxSpriteCount(maxSpriteCount),
          m_spriteBatch(maxSpriteCount),
          m_spriteCount(0)
    {
        m_sprites.reserve(maxSpriteCount);
        m_spritesValidArray.reserve(maxSpriteCount);
    }

    void SpriteSubsystem::Tick(math::FixedPoint dt)
    {
        // NOOP?
    }

    Sprite* SpriteSubsystem::CreateComponent()
    {
        if (m_spriteCount >= m_maxSpriteCount)
        {
            system::LogAndFail(
                "Reached the max number of sprites: " +
                std::to_string(m_maxSpriteCount));
        }

        const size_t BadIndex = static_cast<size_t>(-1);
        size_t nextIndex = BadIndex;
        for (size_t i = 0; i < m_spritesValidArray.size(); ++i)
        {
            if (!m_spritesValidArray[i])
            {
                nextIndex = i;
                break;
            }
        }

        ++m_spriteCount;
        if (nextIndex == BadIndex)
        {
            m_sprites.emplace_back();
            m_spritesValidArray.push_back(true);
            return &m_sprites.back();
        }
        else
        {
            m_spritesValidArray[nextIndex] = true;
            return &m_sprites[nextIndex];
        }
    }

    void SpriteSubsystem::ReleaseComponent(Sprite* pSprite)
    {
        const size_t Index =
            static_cast<size_t>(pSprite - m_sprites.data());
        if (Index >= m_sprites.size())
        {
            system::LogAndFail(
                "SpriteSubsystem::ReleaseComponent: bad pointer arg");
        }

        m_spritesValidArray[Index] = false;
        m_spriteCount--;
    }

    void SpriteSubsystem::Draw()
    {
        // TODO: This belongs in a camera?
        static const glm::mat4 OrthoMatrix =
            glm::ortho(
                0.0f, 
                static_cast<float>(m_window.GetWidth()),
                static_cast<float>(m_window.GetHeight()),
                0.0f,
                -1.0f, 1.0f);

        m_spriteShader.Use();
        m_spriteShader.SetUniformProjMatrix(OrthoMatrix);
        glm::mat4 identity(1.0f);
        m_spriteShader.SetUniformModelMatrix(identity);

        m_spriteBatch.Reset();

        for (size_t i = 0; i < m_sprites.size(); ++i)
        {
            if (!m_spritesValidArray[i]) { continue; }

            if (!m_spriteBatch.GetTexturePtr())
            {
                m_spriteBatch.SetTexturePtr(m_sprites[i].GetTexturePtr());
            }

            m_spriteBatch.AddSprite(m_sprites[i]);
        }

        m_spriteBatch.Draw(&m_spriteShader);
    }
}}
