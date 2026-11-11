#pragma once

#include "gui/ModalBuilder.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

class [[nodiscard]] GuiBuilderHelper final
{
public:
    static tgui::Panel::Ptr createSemitransparentBlackPanel()
    {
        auto panel = tgui::Panel::create();
        panel->setRenderer(
            tgui::Theme::getDefault()->getRenderer("SemitransparentDarkPanel"));
        return panel;
    }

    static ModalBuilderOptions
    getCommonModalBuilderOptions(tgui::Layout2d size, bool scrollable = false)
    {
        return ModalBuilderOptions {
            .modalSize = size,
            .titleBarColor = COLOR_MID_BLUE,
            .bodyScrollable = scrollable,
            .bodyPadding = { 20.f, 20.f },
            .bodyPanelClass =
                scrollable ? "AllCornersPanelScrollable" : "AllCornersPanel",
            .navbarPanelClass = "BottomCutoutPanel",
        };
    }
};
