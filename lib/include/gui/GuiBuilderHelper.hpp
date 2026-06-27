#pragma once

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

class [[nodiscard]] GuiBuilderHelper final
{
public:
    static void centerWidget(tgui::Container::Ptr widget)
    {
        widget->setPosition({ "parent.width / 2 - width / 2",
                              "parent.height / 2 - height / 2" });
    }

    static tgui::Panel::Ptr createSemitransparentBlackPanel()
    {
        auto panel = tgui::Panel::create();
        panel->setRenderer(
            tgui::Theme::getDefault()->getRenderer("SemitransparentDarkPanel"));
        return panel;
    }
};
