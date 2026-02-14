#include "gui/builders/ButtonListBuilder.hpp"
#include "gui/Sizers.hpp"
#include "gui/builders/WidgetBuilder.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <ranges>

ButtonListBuilder& ButtonListBuilder::addButton(
    const StringId labelId,
    std::function<void(void)> onClick,
    const std::string& buttonId)
{
    buttonProps.emplace_back(strings.getString(labelId), onClick, buttonId);
    return *this;
}

tgui::Container::Ptr ButtonListBuilder::build(
    tgui::HorizontalAlignment alignment, tgui::VerticalAlignment vAlignment)
{
    auto&& layout = tgui::GrowVerticalLayout::create();
#ifdef ANDROID
    layout->setSize({ "90%", "100%" });
#else
    layout->setSize({ "50%", "100%" });
#endif

    const std::string& horizontalPosition = [&]
    {
        if (alignment == tgui::HorizontalAlignment::Center)
            return "parent.width / 2 - width / 2";
        else if (alignment == tgui::HorizontalAlignment::Right)
            return "parent.width - width";
        return "0%";
    }();
    layout->getRenderer()->setSpaceBetweenWidgets(
        static_cast<float>(sizer.getBaseFontSize()));

    for (auto&& [idx, props] : std::views::enumerate(buttonProps))
    {
        auto group = tgui::Group::create(
            { "100%", sizer.getBaseContainerHeight() * 1.5f });
        auto&& button = WidgetBuilder::createButton(
            props.label, props.onClick, sizer, player);
        group->add(button, props.buttonId);

        layout->add(group);
    }

    const std::string& verticalPosition = [&]
    {
        if (vAlignment == tgui::VerticalAlignment::Center)
            return "parent.height / 2 - height / 2";
        else if (vAlignment == tgui::VerticalAlignment::Bottom)
            return "parent.height - height";
        return "0%";
    }();

    layout->setPosition(
        { horizontalPosition.c_str(), verticalPosition.c_str() });

    return layout;
}
