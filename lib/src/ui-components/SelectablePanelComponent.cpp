#include "ui-components/SelectablePanelComponent.hpp"

tgui::Panel::Ptr SelectablePanelComponent::buildLayout(DependencyContainer& dic)
{
    auto&& panel = tgui::Panel::create();

    panel->setRenderer(
        tgui::Theme::getDefault()->getRenderer("SelectablePanel"));

    panel->onMouseEnter(
        [panel = panel, &dic]
        {
            dic.soundPlayer.playWidgetHoverEffect();
            panel->setRenderer(
                tgui::Theme::getDefault()->getRenderer("SelectablePanelHover"));
        });

    panel->onMouseLeave(
        [panel = panel]
        {
            panel->setRenderer(
                tgui::Theme::getDefault()->getRenderer("SelectablePanel"));
        });

    return panel;
}
