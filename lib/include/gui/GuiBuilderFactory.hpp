#pragma once

#include "audio/SoundPlayer.hpp"
#include "gui/Sizers.hpp"
#include "gui/builders/ButtonListBuilder.hpp"
#include "gui/builders/DefaultLayoutBuilder.hpp"
#include "gui/builders/FormBuilder.hpp"
#include "gui/builders/NavbarLayoutBuilder.hpp"
#include "gui/builders/TabbedLayoutBuilder.hpp"
#include "gui/builders/TableBuilder.hpp"
#include "gui/builders/WidgetBuilder.hpp"
#include "misc/Compatibility.hpp"
#include "strings/StringProvider.hpp"
#include <type_traits>

template<class E>
concept EnumType = std::is_scoped_enum_v<E>;

class [[nodiscard]] GuiBuilderFactory final
{
public:
    GuiBuilderFactory(
        const dgm::ResourceManager& resmgr,
        const Sizer& sizer,
        const StringProvider& strings,
        SoundPlayer& player) noexcept
        : resmgr(resmgr), sizer(sizer), strings(strings), player(player)
    {
    }

    GuiBuilderFactory(GuiBuilderFactory&&) = delete;
    GuiBuilderFactory(const GuiBuilderFactory&) = delete;

public:
    ButtonListBuilder createButtonListBuilder() const
    {
        return ButtonListBuilder(strings, sizer, player);
    }

    DefaultLayoutBuilder createDefaultLayoutBuiler() const
    {
        return DefaultLayoutBuilder(sizer);
    }

    FormBuilder createFormBuilder() const
    {
        return FormBuilder(strings, sizer);
    }

    NavbarLayoutBuilder createNavbarLayoutBuilder() const
    {
        return NavbarLayoutBuilder(sizer);
    }

    TabbedLayoutBuilder createTabbedLayoutBuilder() const
    {
        return TabbedLayoutBuilder(strings, sizer, player);
    }

    TableBuilder createTableBuilder() const
    {
        return TableBuilder(sizer);
    }

    tgui::Button::Ptr createIconButton(EnumType auto icon, auto&& callback)
    {
        return WidgetBuilder::createTexturedButton(
            resmgr.get<tgui::Texture>(
                uni::format("Icon-{}", std::to_underlying(icon))),
            std::forward<decltype(callback)>(callback),
            player);
    };

    tgui::Button::Ptr createTextButton(StringId textId, auto&& callback)
    {
        return WidgetBuilder::createButton(
            strings.getString(textId),
            std::forward<decltype(callback)>(callback),
            sizer,
            player);
    }

private:
    const dgm::ResourceManager& resmgr;
    const Sizer& sizer;
    const StringProvider& strings;
    SoundPlayer& player;
};
