#pragma once

#include <sputter/assets/resourcestoragetype.h>
#include <array>

namespace sputter { namespace assets {
    class AssetStorage;

    class AssetStorageProvider
    {
    public:   
        AssetStorageProvider(AssetStorage* pGeneralStorage)
            : m_pGeneralStorage(pGeneralStorage)
        {}

        AssetStorage* GetGeneralStorage() { return m_pGeneralStorage; }

        template<typename S>
        void AddResourceStorageByType(S* pStorageType)
        {
            m_storageArray[S::StorageTypeId] = pStorageType;
        }

        template<typename S>
        S* GetStorageByType()
        {
            return reinterpret_cast<S*>(m_storageArray[S::StorageTypeId]);
        }

    private:   
        AssetStorageProvider() = delete;
        AssetStorageProvider& operator=(const AssetStorageProvider& other) = delete;

        std::array<void*, ResourceStorageType::TYPE_MAX> m_storageArray;
        AssetStorage*                                    m_pGeneralStorage;
    };
}}