#pragma once

#include "element.h"

#include <cstdint>

#include <sputter/render/color.h>

namespace sputter { namespace ui {
    class Button : public Element
    {
    public:
        Button(Element* pParent);

        virtual void HandleEvent(uint8_t eventCode, void* pEventData) override;

        void SetBorderSize(uint8_t borderSize);

        void SetBorderColor(uint8_t r, uint8_t g, uint8_t b);
        void SetBorderColor(const render::Color& color);

    protected:
        virtual void DrawInternal() override;
        virtual void TickInternal(float dt) override;

    private:
        uint8_t       m_borderSize = 1;
        render::Color m_borderColor = render::Color::White;
    };
}}
