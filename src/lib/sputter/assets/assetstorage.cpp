#include "assetstorage.h"

#include "imagedata.h"
#include "pngreader.h"

#include "textdata.h"
#include "textfilereader.h"

#include "binarydata.h"
#include "binaryfilereader.h"

#include <algorithm>
#include <filesystem>
#include <string>
#include <cstring>

#include <sputter/system/system.h>
#include <sputter/log/log.h>

namespace 
{
    bool IsTextFileExtension(const std::filesystem::path& extensionPath)
    {
        const std::string Extension = extensionPath.string();
        static const std::vector<std::string> TextFileExtensions = {
            ".vert", ".frag", ".glsl"
        };

        const auto it = std::find(std::cbegin(TextFileExtensions), std::cend(TextFileExtensions), Extension);
        return it != std::end(TextFileExtensions);
    }

    bool IsImageFileExtension(const std::filesystem::path& extensionPath)
    {
        const std::string Extension = extensionPath.string();

        // S'all we support atm
        return Extension == ".png";
    }

    bool IsBinaryFileExtension(const std::filesystem::path& extensionPath)
    {
        const std::string Extension = extensionPath.string();

        // S'all we support atm
        return Extension == ".ttf";
    }
}

namespace sputter { namespace assets {
    AssetStorage::AssetStorageEntry::AssetStorageEntry(
        const std::string& name,
        const std::string& relativePath,
        std::shared_ptr<AssetData> spData)
    {
        strncpy(Name, name.c_str(), sizeof(Name));
        strncpy(RelativePath, relativePath.c_str(), sizeof(RelativePath));
        spAssetData = spData;
    }

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
            if (IsImageFileExtension(CurrentPath.extension()))
            {
                PngReader reader;

                const std::string& relativePathString = 
                    CurrentPath.string().substr(rootPathStr.length() + 1);
                auto spData = std::make_shared<ImageData>();
                if (reader.ReadImage(CurrentPath.string(), spData.get()))
                {
                    RELEASE_LOG_INFO(LOG_ASSETS, "Asset loader: loaded %s", relativePathString.c_str());
                    m_assetMap[m_numAssets++] = AssetStorageEntry(
                        relativePathString,
                        CurrentPath.stem().string(),
                        spData
                    );
                }
                else
                {
                    RELEASE_LOG_WARNING(LOG_ASSETS, "Asset loader: failed to load %s", relativePathString.c_str());
                }
            }
            else if (IsTextFileExtension(CurrentPath.extension()))
            {
                TextFileReader reader;

                const std::string& relativePathString = 
                    CurrentPath.string().substr(rootPathStr.length() + 1);
                auto spData = std::make_shared<TextData>();
                if (reader.ReadTextFile(CurrentPath.string(), spData.get()))
                {
                    RELEASE_LOG_INFO(LOG_ASSETS, "Asset loader: loaded %s", relativePathString.c_str());
                    m_assetMap[m_numAssets++] = AssetStorageEntry(
                        relativePathString,
                        CurrentPath.stem().string(),
                        spData
                    );
                }
                else
                {
                    RELEASE_LOG_WARNING(LOG_ASSETS, "Asset loader: failed to load %s", relativePathString.c_str());
                }
            }
            else if (IsBinaryFileExtension(CurrentPath.extension()))
            {
                BinaryFileReader reader;

                const std::string& relativePathString = 
                    CurrentPath.string().substr(rootPathStr.length() + 1);
                auto spData = std::make_shared<BinaryData>();
                if (reader.ReadBinaryFile(CurrentPath.string(), spData.get()))
                {
                    RELEASE_LOG_INFO(LOG_ASSETS, "Asset loader: loaded %s", relativePathString.c_str());
                    m_assetMap[m_numAssets++] = AssetStorageEntry(
                        relativePathString,
                        CurrentPath.stem().string(),
                        spData
                    );
                }
                else
                {
                    RELEASE_LOG_WARNING(LOG_ASSETS, "Asset loader: failed to load %s", relativePathString.c_str());
                }
            }
        }
    }

    std::shared_ptr<AssetData> 
    AssetStorage::FindFirstByName(const std::string& assetName) const
    {
        for (size_t i = 0; i < m_numAssets; ++i)
        {
            const AssetStorageEntry& StorageEntry = m_assetMap[i];
            if (assetName == StorageEntry.RelativePath)
            {
                return StorageEntry.spAssetData;
            }
        }

        return std::shared_ptr<AssetData>(nullptr);
    }
}}
