#pragma once

#include "element.h"

#include <cstdint>
#include <functional>

namespace sputter 
{
    namespace render
    {
        class VolumetricTextRenderer;
    }

    namespace math
    {
        class Vector2i;
    }
}

namespace sputter { namespace ui {
    struct Event;
    struct Theme;
    class Button;

    // TODO: Font rendering should be statically available. Also, shouldn't use a volumetric
    // text renderer.
    void SetFontRenderer(render::VolumetricTextRenderer* pTextRenderer);

    class ModalPopup : public Element
    {
    public:
        // A bit rigid, but this'll do for now.
        static const uint8_t kMaxModalPopupSelectionOptions = 2;
        enum class ModalPopupSelection 
        {
            Invalid = 0,
            Cancelled,
            Selection_0,
            Selection_1,
            MaxSelection
        };

        ModalPopup(
            Element* pParent, Theme* pTheme,
            render::VolumetricTextRenderer* pTextRenderer,
            const math::Vector2i& position, const math::Vector2i& dimensions,
            const math::Vector2i& buttonDimensions, // Not interested in automating layout and content fitting atm
            const char** ppButtonTextEntries, uint32_t numButtonTextEntries,
            const char* pText = nullptr);

        virtual ~ModalPopup();
        virtual void HandleEvent(uint8_t eventCodeParameter, void* pEventData) override;

        using ModalPopupOptionSelectedCallback = std::function<void(ModalPopupSelection)>;
        void SetModalPopupOptionSelectedCallback(const ModalPopupOptionSelectedCallback onOptionSelected);

        void SetText(const char* pText);

    protected:
        virtual void DrawInternal() override;
        virtual void TickInternal(float dt) override;

    private:
        void OnButtonPressed(uint8_t buttonIndex);

        Button**                        m_ppButtonArray = nullptr;
        uint8_t                         m_numButtons    = 0;

        Theme*                          m_pTheme        = nullptr;
        render::VolumetricTextRenderer* m_pTextRenderer = nullptr;
        const char*                     m_pText         = nullptr;

        ModalPopupOptionSelectedCallback m_fnOptionSelectedCallback;

        // Probably parameters we want up in Element, but that sort of generalization isn't needed right now.
        static const int32_t kDefaultVerticalMargin = 5;
        int32_t             m_verticalMargin = kDefaultVerticalMargin;

        // Strictly adjusts the spacing between buttons.
        static const int32_t kDefaultInterButtonPadding = 30;
        int32_t             m_horizontalInterButtonPadding = kDefaultInterButtonPadding;
    };
}}