#include <sputter/assets/imagedata.h>
#include <sputter/assets/assetstorage.h>

#include <sputter/render/spritesubsystem.h>
#include <sputter/render/sprite.h>
#include <sputter/render/texturestorage.h>
#include <sputter/render/window.h>

#include <sputter/physics/rigidbodysubsystem.h>
#include <sputter/physics/rigidbody2d.h>

#include <sputter/system/system.h>
#include <sputter/system/time.h>

#include <iostream>

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

    physics::RigidBodySubsystemSettings rigidBodySettings;
    rigidBodySettings.MaxRigidBodies = 5;

    physics::RigidBodySubsystem rigidBodySubsystem(rigidBodySettings);
    physics::RigidBody2D* pMainRigidBody = rigidBodySubsystem.CreateComponent();

    const math::FixedPoint FPThreeFifty(350);
    pMainRigidBody->Position.Set(FPThreeFifty, FPThreeFifty);

    render::SpriteSubsystem spriteSubsystem(window, 10 /* max sprites */);
    render::Sprite* pSprite = spriteSubsystem.CreateComponent();
    pSprite->SetPosition(pMainRigidBody->Position);
    pSprite->SetTexturePtr(spTexture);
    pSprite->SetDimensions(50.0f, 50.0f);
    
    window.EnableInputs();

    const math::FixedPoint DesiredFps  = math::FPSixty;
    const math::FixedPoint FrameStepMs = math::FixedPoint(1000) / DesiredFps;
    const math::FixedPoint DeltaTime = math::FPOne / DesiredFps;
    uint32_t nextTick = 
        system::GetTickMilliseconds() + static_cast<uint32_t>(FrameStepMs);
    while (!window.ShouldClose() && !window.GetKeyState(GLFW_KEY_ESCAPE))
    {
        window.Clear();

        rigidBodySubsystem.Tick(DeltaTime);

        pSprite->SetPosition(pMainRigidBody->Position);

        spriteSubsystem.Draw();

        window.Tick();

        const uint32_t TimeMs = system::GetTickMilliseconds();
        if (TimeMs < nextTick)
        {
            system::SleepMs(nextTick - TimeMs);
        }
        nextTick = TimeMs + static_cast<uint32_t>(FrameStepMs);
    }

    rigidBodySubsystem.ReleaseComponent(pMainRigidBody);

    return 0;
}
