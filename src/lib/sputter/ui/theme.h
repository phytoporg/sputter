#pragma once

#include <sputter/render/color.h>

namespace sputter { namespace ui {
    struct Theme 
    {
        // General
        render::Color UnfocusedBorderColor;
        render::Color FocusedBorderColor;

        // Buttons
        // TODO: Render button backgrounds. Not going to work with these until that's
        // in place.
        //
        // render::Color ButtonDownBorderColor;
        // render::Color ButtonDownBackgroundColor;
    };
}}
