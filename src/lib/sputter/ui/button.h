#pragma once

#include "element.h"
#include "theme.h"
#include "navigation.h"

#include <cstdint>

#include <sputter/render/color.h>

namespace sputter { namespace ui {
    struct Event;
    class Button : public Element
    {
    public:
        Button(Element* pParent, Theme* pTheme, const char* pText = nullptr);

        virtual void HandleEvent(uint8_t eventCode, void* pEventData) override;

        void SetBorderSize(uint8_t borderSize);
        
        // Not confident this is a great way to accomplish this, but at least it's simple.
        void SetNavigationLink(Button* pOtherButton, NavigationDirections direction);

    protected:
        virtual void DrawInternal() override;
        virtual void TickInternal(float dt) override;

    private:
        void SignalRootElement(const Event& event);

        uint8_t       m_borderSize = 1;
        render::Color m_borderColor = render::Color::White;

        Theme*        m_pTheme = nullptr; 
        Button*       m_pNavLinks[4];

        static const uint32_t kMaxTextLength = 256;
        char m_text[kMaxTextLength];
    };
}}
