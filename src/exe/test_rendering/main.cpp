#include <sputter/assets/imagedata.h>
#include <sputter/assets/assetstorage.h>

#include <sputter/render/window.h>
#include <sputter/render/spritebatch.h>
#include <sputter/render/sprite.h>
#include <sputter/render/spriteshader.h>
#include <sputter/render/texturestorage.h>

#include <sputter/system/system.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace sputter;

int main(int argc, char** argv)
{
    system::InitializeLogging(argv[0]);

    if (argc < 3)
    {
        std::cerr << "Usage: " 
                  << argv[0] << " "
                  << "<asset_path> <test image asset>" 
                  << std::endl;
        return -1;
    }

    assets::AssetStorage assetStorage(argv[1]);
    auto spAsset = assetStorage.FindFirstByName(argv[2]);

    if (!spAsset)
    {
        LOG(ERROR) << "Could not find asset" << std::endl;
        return -1;
    }

    if (spAsset->GetType() != assets::IMAGE_ASSET)
    {
        LOG(ERROR) << "Asset is not the correct type" << std::endl;
        return -1;
    }

    auto pImageData = dynamic_cast<assets::ImageData*>(spAsset.get());
    if (!pImageData->pBytes)
    {
        LOG(ERROR) << "No bytes!" << std::endl;
        return -1;
    }

    LOG(INFO) << "Found asset " << argv[2] << "!" << std::endl;

    render::Window window("Test window");

    render::TextureStorage textureStorage;
    if (!textureStorage.AddTexture(*pImageData, argv[2]))
    {
        LOG(ERROR) << "Failed to add texture.";
        return -1;
    }

    render::TexturePtr spTexture;
    if (!(spTexture = textureStorage.FindTextureByName(argv[2])))
    {
        LOG(ERROR) << "Couldnt find the texture we just added.";
        return -1;
    }
    
    // create sprite
    render::Sprite sprite(spTexture, 64, 32);
    
    // create sprite batch
    render::SpriteBatch spriteBatch(spTexture, 1);
    
    // create shader
    render::SpriteShader spriteShader;

    glm::mat4 orthoMatrix = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f);
    spriteShader.SetUniformProjMatrix(orthoMatrix);

    glm::mat4 identity(1.0f);
    spriteShader.SetUniformModelMatrix(identity);

    window.EnableInputs();
    while (!window.ShouldClose() && !window.GetKeyState(GLFW_KEY_ESCAPE))
    {
        window.Clear();
        window.Tick();

        spriteShader.Use();

        spriteBatch.Reset();
        spriteBatch.AddSprite(sprite);
        spriteBatch.Draw();
    }

    return 0;
}
