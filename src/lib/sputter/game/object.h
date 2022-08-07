#pragma once

#include "subsystemtype.h"
#include "subsystemprovider.h"

#include <array>

namespace sputter { namespace game {
    class Object
    {
    public:
        Object(SubsystemProvider* pProvider);

        virtual void Tick(float dt) = 0;

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
        void CreateAndSetComponentByType(ComponentType** ppComponentOut)
        {
            auto pSubsystem = m_pSubsystemProvider->GetSubsystemByType<SubsystemType>();
            ComponentType* pComponent = pSubsystem->CreateComponent();
            if (ppComponentOut)
            {
                *ppComponentOut = pComponent;
            }

            SetComponentByType(pComponent);
        }


    protected:
        sputter::game::SubsystemProvider* m_pSubsystemProvider;

    private:
        Object() = delete;
        Object(const Object& other) = delete;

        std::array<void*, SubsystemType::TYPE_MAX> m_components;
    };
}}
