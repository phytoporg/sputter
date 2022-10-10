#pragma once

#include "subsystemtype.h"

#include <sputter/core/component.h>
#include <sputter/game/subsystemprovider.h>
#include <sputter/math/fixedpoint.h>
#include <sputter/system/system.h>

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
            uint32_t objectType,
            sputter::assets::AssetStorageProvider* pAssetProvider);

        virtual void Tick(math::FixedPoint deltaTime) {}
        virtual void PostTick(math::FixedPoint deltaTime) {}

        template<typename ComponentType>
        ComponentType* GetComponentByType()
        {
            const SubsystemType Lookup = ComponentType::ComponentId;
            return reinterpret_cast<ComponentType*>(m_components[Lookup]);
        }

        template<typename ComponentType>
        core::ComponentHandle* GetComponentHandleByType()
        {
            ComponentType* pComponent = GetComponentByType<ComponentType>();
            return pComponent->GetComponentHandle(pComponent);
        }

        template<typename ComponentType>
        void SetComponentByType(ComponentType* pComponent)
        {
            m_components[ComponentType::ComponentId] = pComponent;
        }

        template<typename SubsystemType, typename ComponentType>
        void CreateAndSetComponentByType(ComponentType** ppComponentOut, const typename ComponentType::InitializationParameters& params)
        {
            auto pSubsystemProvider = SubsystemProvider::GetSubsystemProviderAddress();
            auto pSubsystem = pSubsystemProvider->GetSubsystemByType<SubsystemType>();
            ComponentType* pComponent = pSubsystem->CreateComponent(params);
            if (ppComponentOut)
            {
                *ppComponentOut = pComponent;
            }

            SetComponentByType(pComponent);
        }

        template<typename SubsystemType>
        sputter::core::ComponentHandle CreateAndSetComponentByType(const typename SubsystemType::Component::InitializationParameters& params)
        {
            auto pSubsystemProvider = SubsystemProvider::GetSubsystemProviderAddress();
            auto pSubsystem = pSubsystemProvider->GetSubsystemByType<SubsystemType>();
            typename SubsystemType::Component* pComponent = pSubsystem->CreateComponent(params);
            if (pComponent)
            {
                SetComponentByType(pComponent);
                return pSubsystem->GetComponentHandle(pComponent);
            }
            else
            {
                system::LogAndFail("Failed to create and set component by type");
                return sputter::core::kInvalidComponentHandle; // Appease the compiler
            }
        }

        uint32_t GetType() const;

    protected:
        sputter::assets::AssetStorageProvider* m_pAssetStorageProvider;

    private:
        Object() = delete;
        Object(const Object& other) = delete;

        uint32_t m_objectType;
        std::array<void*, SubsystemType::TYPE_MAX> m_components;
    };
}}
