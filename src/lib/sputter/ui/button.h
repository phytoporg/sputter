#pragma once

#include "element.h"
#include "theme.h"
#include "navigation.h"

#include <cstdint>
#include <functional>

#include <sputter/render/color.h>
#include <sputter/render/volumetrictext.h>

namespace sputter 
{
    namespace render
    {
        class VolumetricTextRenderer;
    }
}

namespace sputter { namespace ui {
    struct Event;
    class Button : public Element
    {
    public:
        Button(Element* pParent, Theme* pTheme, const char* pText = nullptr);

        virtual void HandleEvent(uint8_t eventCodeParameter, void* pEventData) override;

        void SetBorderSize(uint8_t borderSize);
        
        // Not confident this is a great way to accomplish this, but at least it's simple.
        void SetNavigationLink(Button* pOtherButton, NavigationDirections direction);

        // TODO: Font rendering should be statically available. Also, shouldn't use a volumetric
        // text renderer.
        void SetFontRenderer(render::VolumetricTextRenderer* pTextRenderer);

        using ButtonPressedCallback = std::function<void()>;
        void SetButtonPressedCallback(const ButtonPressedCallback onButtonPressed);

        void SetButtonIsDisabled(bool isDisabled);

    protected:
        virtual void DrawInternal() override;
        virtual void TickInternal(float dt) override;

    private:
        void SignalRootElement(const Event& event);
        enum class ButtonState : uint8_t {
            Invalid = 0,
            Idle,
            Down,
            ButtonStateMax
        };
        ButtonState                     m_buttonState = ButtonState::Idle;
        bool                            m_buttonDisabled = false;

        ButtonPressedCallback           m_fnButtonPressed;

        uint8_t                         m_borderSize  = 1;
        render::Color                   m_borderColor = render::Color::White;

        Theme*                          m_pTheme      = nullptr; 
        Button*                         m_pNavLinks[4];
        render::VolumetricTextRenderer* m_pTextRenderer;

        static const uint32_t kMaxTextLength = 256;
        char m_text[kMaxTextLength];
    };
}}
