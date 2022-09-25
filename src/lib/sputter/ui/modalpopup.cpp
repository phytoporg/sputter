#include "modalpopup.h"
#include "theme.h"
#include "button.h"
#include "events.h"

#include <sputter/core/check.h>
#include <sputter/math/vector2i.h>
#include <sputter/render/drawshapes.h>

using namespace sputter::render;
using namespace sputter::ui;
using namespace sputter::math;

ModalPopup::ModalPopup(
    Element* pParent, Theme* pTheme,
    render::VolumetricTextRenderer* pTextRenderer,
    const math::Vector2i& position, const math::Vector2i& dimensions,
    const math::Vector2i& buttonDimensions,
    const char** ppButtonTextEntries, uint32_t numButtonTextEntries,
    const char* pTitle)
    : Element(pParent),
      m_pTheme(pTheme),
      m_pTextRenderer(pTextRenderer),
      m_pTitle(pTitle)
{
    RELEASE_CHECK(numButtonTextEntries <= kMaxModalPopupSelectionOptions, "Too many button entires for a modal popup");

    SetPosition(position);
    SetDimensions(dimensions);

    m_numButtons = numButtonTextEntries;
    m_ppButtonArray = new Button*[m_numButtons];

    // Straight up fail if there isn't enough room for the buttons.
    const int32_t TotalHorizontalButtonSpace = 
        buttonDimensions.GetX() * m_numButtons + 
        m_horizontalInterButtonPadding * (m_numButtons - 1);
    {
        RELEASE_CHECK(TotalHorizontalButtonSpace <= dimensions.GetX(), "Not enough horizontal space in modal to accommodate buttons");

        const int32_t TotalVerticalButtonSpace = buttonDimensions.GetY() + m_verticalMargin;
        RELEASE_CHECK(TotalVerticalButtonSpace <= dimensions.GetY(), "Not enough vertical space in modal to accommodate buttons");
    }

    const int32_t ButtonOffsetY = buttonDimensions.GetY() - m_verticalMargin;

    // Center the buttons!
    int32_t currentButtonOffsetX = (dimensions.GetX() - TotalHorizontalButtonSpace) / 2;

    Button* pPreviousButton = nullptr;
    for (uint8_t i = 0; i < m_numButtons; ++i)
    {
        RELEASE_CHECK(ppButtonTextEntries[i], "All button text entries must be non-null");

        Button* pCurrentButton = new Button(this, m_pTheme, ppButtonTextEntries[i]);
        pCurrentButton->SetFontRenderer(m_pTextRenderer);
        pCurrentButton->SetPosition(currentButtonOffsetX, ButtonOffsetY);
        pCurrentButton->SetDimensions(buttonDimensions);

        if (pPreviousButton)
        {
            pPreviousButton->SetNavigationLink(pCurrentButton, NavigationDirections::Right);
            pCurrentButton->SetNavigationLink(pPreviousButton, NavigationDirections::Left);
        }

        currentButtonOffsetX += m_horizontalInterButtonPadding + buttonDimensions.GetX();

        pPreviousButton = pCurrentButton;
        m_ppButtonArray[i] = pCurrentButton;
    }

    // Default focus on the first button, 'cause why not
    SignalRootElement(Event{ EventCode::FocusBegin, m_ppButtonArray[0] });
}

ModalPopup::~ModalPopup()
{
    if (m_ppButtonArray)
    {
        for (uint8_t i = 0; i < m_numButtons; ++i)
        {
            delete m_ppButtonArray[i];
        }
        delete[] m_ppButtonArray;
    }
}

void ModalPopup::HandleEvent(uint8_t eventCodeParameter, void* pEventData)
{}

void ModalPopup::SetModalPopupOptionSelectedCallback(const ModalPopupOptionSelectedCallback onOptionSelected)
{}

void ModalPopup::DrawInternal()
{
    const float RenderDepth = -1.0f * GetElementDepth();
    const float PreviousLineRenderDepth = shapes::GetLineRendererDepth();
    shapes::SetLineRendererDepth(RenderDepth);

    const auto AbsolutePosition = GetAbsolutePosition();
    const auto Dimensions = GetDimensions();
    shapes::DrawFilledRect(
        GetAbsolutePosition(), GetDimensions(),
        m_pTheme->ModalBorderSize, m_pTheme->FocusedBorderColor,
        m_pTheme->ModalBackgroundColor);

    shapes::SetLineRendererDepth(PreviousLineRenderDepth);
}

void ModalPopup::TickInternal(float dt)
{}