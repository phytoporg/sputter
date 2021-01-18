#include "assetstorage.h"
#include "pngreader.h"
#include <filesystem>
#include <sputter/system/system.h>

namespace sputter { namespace assets {
    AssetStorage::AssetStorage(const std::string& rootPathStr)
    {
        namespace fs = std::filesystem;

        const fs::path RootPath(rootPathStr);
        if (!fs::exists(RootPath) || !fs::is_directory(RootPath))
        {
            system::LogAndFail("Invalid asset storage root path.");
        }

        for (auto dirEntry : fs::recursive_directory_iterator(RootPath))
        {
            const fs::path CurrentPath = dirEntry.path();
            if (fs::is_directory(CurrentPath))
            {
                continue;
            };

            // We only support .png files at the moment for image assets
            if (CurrentPath.extension() == ".png")
            {
                PngReader reader;

                const std::string& relativePathString = 
                    CurrentPath.string().substr(rootPathStr.length() + 1);
                auto spData = std::make_shared<ImageData>();
                if (reader.ReadImage(CurrentPath.string(), spData.get()))
                {
                    LOG(INFO) << "Asset loader: loaded " 
                              << relativePathString;

                    m_assetMap.insert({
                        relativePathString,
                        {CurrentPath.stem().string(), spData}
                        });
                }
                else
                {
                    LOG(WARNING) << "Asset loader: failed to load " 
                                 << CurrentPath.string();
                }
            }
        }
    }

    std::shared_ptr<AssetData> 
    AssetStorage::FindFirstByName(const std::string& assetName) const
    {
        for (auto& mapPair : m_assetMap)
        {
            const AssetStorageEntry& storageEntry = mapPair.second;
            LOG(INFO) << storageEntry.Name;
            if (assetName == storageEntry.Name)
            {
                return storageEntry.spAssetData;
            }
        }

        return std::shared_ptr<AssetData>(nullptr);
    }
}}
