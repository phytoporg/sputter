#pragma once

#include <sputter/render/color.h>

#include <cstdint>

namespace sputter { namespace ui {
    struct Theme 
    {
        // General
        render::Color UnfocusedBorderColor;
        render::Color FocusedBorderColor;

        // Buttons
        uint8_t ButtonBorderSize = 1;
        render::Color ButtonDownAndDisabledBorderColor;
        // TODO: Render button backgrounds. Not going to work with these until that's
        // in place.
        //
        // render::Color ButtonDownBorderColor;
        // render::Color ButtonDownBackgroundColor;
    
        // Modals
        uint8_t ModalBorderSize = 1;
    };
}}
