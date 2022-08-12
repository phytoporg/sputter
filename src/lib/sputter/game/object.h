#pragma once

#include "subsystemtype.h"

#include <sputter/game/subsystemprovider.h>
#include <sputter/math/fixedpoint.h>

#include <array>

namespace sputter { namespace assets {
    class AssetStorageProvider;
}}

namespace sputter { namespace game {
    class SubsystemProvider;

    class Object
    {
    public:
        Object(
            sputter::assets::AssetStorageProvider* pAssetProvider,
            SubsystemProvider* pSubsystemProvider);

        virtual void Tick(math::FixedPoint deltaTime) = 0;

        template<typename ComponentType>
        ComponentType* GetComponentByType()
        {
            const SubsystemType Lookup = ComponentType::ComponentId;
            return reinterpret_cast<ComponentType*>(m_components[Lookup]);
        }

        template<typename ComponentType>
        void SetComponentByType(ComponentType* pComponent)
        {
            m_components[ComponentType::ComponentId] = pComponent;
        }

        template<typename SubsystemType, typename ComponentType>
        void CreateAndSetComponentByType(ComponentType** ppComponentOut, const typename ComponentType::InitializationParameters& params)
        {
            auto pSubsystem = m_pSubsystemProvider->GetSubsystemByType<SubsystemType>();
            ComponentType* pComponent = pSubsystem->CreateComponent(params);
            if (ppComponentOut)
            {
                *ppComponentOut = pComponent;
            }

            SetComponentByType(pComponent);
        }

    protected:
        sputter::game::SubsystemProvider*      m_pSubsystemProvider;
        sputter::assets::AssetStorageProvider* m_pAssetStorageProvider;

    private:
        Object() = delete;
        Object(const Object& other) = delete;

        std::array<void*, SubsystemType::TYPE_MAX> m_components;
    };
}}
