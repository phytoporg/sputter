#include "assetstorage.h"
#include "pngreader.h"
#include <filesystem>
#include <sputter/system/system.h>

// Helper functions
namespace
{
    bool 
    InitializeAssetMap(
        const std::string& rootPath,
        std::map<std::string, AssetStorageEntry>& assetMap
        )
    {
        namespace filesystem = fs;
        if (!fs::exists(rootPath) || !fs::is_directory(rootPath))
        {
            LOG(ERROR) << "Invalid asset storage root path " << rootPath;
            return false;
        }

        for (fs::path& currentPath : fs::recursive_directory_iterator(rootPath))
        {
            if (fs::is_directory(currentPath))
            {
                continue;
            };

            // We only support .png files at the moment for image assets
            if (currentPath.extension == ".png")
            {
                PngReader reader;

                const std::string& currentPathString = currentPath.string();
                spData = std::make_shared<ImageData>();
                if (reader.ReadImage(currentPathString, spData.get()))
                {
                    LOG(INFO) << "Asset loader: loaded " 
                              << currentPathString;

                    m_assetMap.insert({currentPathString, spData});
                }
                else
                {
                    LOG(WARNING) << "Asset loader: failed to load " 
                                 << currentPath.string();
                }
            }
        }

        return true;
    }
}

namespace sputter { namespace assets {
    AssetStorage::AssetStorage(const std::string& rootPath)
    {
        if (!InitializeAssetMap(rootPath, m_assetMap))
        {
            LogAndFail("Failed to initialize asset map in asset storage.");
        }
    }

    AssetData* AssetStorage::FindFirstByName(const std::string& assetName)
    {
        return nullptr;
    }
}}
