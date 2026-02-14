#pragma once

#include "audio/SoundPlayer.hpp"
#include "gui/Sizers.hpp"
#include "misc/Compatibility.hpp"
#include "strings/StringProvider.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <functional>
#include <string>
#include <vector>

class [[nodiscard]] ButtonListBuilder final
{
public:
    ButtonListBuilder(
        const StringProvider& strings,
        const Sizer& sizer,
        SoundPlayer& player) noexcept
        : strings(strings), sizer(sizer), player(player)
    {
    }

    ButtonListBuilder(const ButtonListBuilder&) = delete;
    ButtonListBuilder(ButtonListBuilder&&) = delete;
    ~ButtonListBuilder() = default;

public:
    ButtonListBuilder& addButton(
        const StringId labelId,
        std::function<void(void)> onClick,
        const std::string& buttonId = "");

    [[nodiscard]] tgui::Container::Ptr build(
        tgui::HorizontalAlignment alignment = tgui::HorizontalAlignment::Center,
        tgui::VerticalAlignment vAlignment = tgui::VerticalAlignment::Center);

private:
    struct ButtonProps
    {
        std::string label;
        std::function<void(void)> onClick;
        std::string buttonId;
    };

    const StringProvider& strings;
    const Sizer& sizer;
    SoundPlayer& player;
    std::vector<ButtonProps> buttonProps;
};
