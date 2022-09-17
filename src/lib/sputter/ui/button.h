#pragma once

#include "element.h"

#include <cstdint>

#include <sputter/render/color.h>

namespace sputter { namespace ui {
    class Button : public Element
    {
    public:
        Button(Element* pParent);

        virtual void Tick(float dt) override;
        virtual void Draw() override;

        virtual void HandleEvent(uint32_t eventCode, void* pEventData) override;

        void SetBorderWidth(uint8_t borderWidth);

        void SetBorderColor(uint8_t r, uint8_t g, uint8_t b);
        void SetBorderColor(const render::Color& color);

    private:
        uint8_t       m_borderWidth;

        render::Color m_borderColor;
    };
}}