#include "spritesubsystem.h"

#include "shader.h"
#include "shaderstorage.h"

#include <sputter/system/system.h>
#include <sputter/log/log.h>

#include <sputter/assets/assetstorage.h>
#include <sputter/assets/textdata.h>

#include <glm/glm.hpp>

using namespace sputter::render;

// TODO(philjo 2/7/2021): Most of this implementation is ripped straight from
// the rigidbody subsystem. Subsystem component lifetime management might 
// benefit from generalization?? We'll see after we implement a few more.
SpriteSubsystem::SpriteSubsystem(
    sputter::assets::AssetStorage* pAssetStorage,
    ShaderStorage* pShaderStorage,
    const SpriteSubsystemSettings& settings)
    : m_maxSpriteCount(settings.MaxSpriteCount),
      m_spriteBatch(m_maxSpriteCount),
      m_spriteCount(0)
{
    m_sprites.reserve(m_maxSpriteCount);
    m_spritesValidArray.reserve(m_maxSpriteCount);

    auto spSpriteVertexShader = pAssetStorage->FindFirstByName(settings.SpriteVertexShaderName);
    auto pSpriteVertexShader = dynamic_cast<assets::TextData*>(spSpriteVertexShader.get());

    auto spSpriteFragmentShader = pAssetStorage->FindFirstByName(settings.SpriteFragmentShaderName);
    auto pSpriteFragmentShader = dynamic_cast<assets::TextData*>(spSpriteFragmentShader.get());
    if (!pShaderStorage->AddShader(*pSpriteVertexShader, *pSpriteFragmentShader, "sprite_shader"))
    {
        RELEASE_LOG_ERROR_(LOG_RENDER, "Could not add sprite shader to storage.");
        return;
    }

    render::ShaderPtr spShader = pShaderStorage->FindShaderByName("sprite_shader");
    if (!spShader)
    {
        RELEASE_LOG_ERROR_(LOG_RENDER, "Failed to find sprite_shader in shader storage");
        return;
    }

    m_pSpriteShader = spShader.get();
}

void SpriteSubsystem::Tick(math::FixedPoint dt)
{
    // NOOP?
}

Sprite* SpriteSubsystem::CreateComponent(const Sprite::InitializationParameters& /*params*/)
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

void SpriteSubsystem::Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix)
{
    m_pSpriteShader->Bind();
    const uint32_t uniformProjMatrixHandle = m_pSpriteShader->GetUniform("projection");
    Uniform<glm::mat4>::Set(uniformProjMatrixHandle, projMatrix);

    const uint32_t uniformViewMatrixHandle = m_pSpriteShader->GetUniform("view");
    Uniform<glm::mat4>::Set(uniformViewMatrixHandle, viewMatrix);

    const uint32_t uniformModelMatrixHandle = m_pSpriteShader->GetUniform("model");
    const glm::mat4 Identity(1.0f);
    Uniform<glm::mat4>::Set(uniformModelMatrixHandle, Identity);

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

    m_spriteBatch.Draw(m_pSpriteShader);
    m_pSpriteShader->Unbind();
}
