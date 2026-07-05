#pragma once

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
};
