#pragma once

#include "audio/SoundPlayer.hpp"
#include "gui/Gui.hpp"
#include "gui/Sizers.hpp"
#include "gui/TguiHelper.hpp"
#include "gui/builders/WidgetBuilder.hpp"
#include "strings/StringId.hpp"
#include "strings/StringProvider.hpp"
#include <optional>
#include <tuple>

namespace detail
{
    struct [[nodiscard]] ModalBuilderDeps final
    {
        const StringProvider<StringId>& strings;
        const Sizer& sizer;
        GuiAudioInterface& soundPlayer;
    };

    template<ScopedEnum StringId>
    class [[nodiscard]] ModalBuilderWithContent final
    {
    public:
        ModalBuilderWithContent(
            ModalBuilderDeps&& deps,
            tgui::ChildWindow::Ptr&& modal,
            tgui::GrowHorizontalLayout::Ptr&& bottomNavbar)
            : deps(std::move(deps))
            , modal(std::move(modal))
            , bottomNavbar(std::move(bottomNavbar))
        {
        }

    public:
        ModalBuilderWithContent<StringId>& withBottomButton(
            StringId label,
            std::function<void(void)> onClick,
            WidgetOptions options = {})
        {
            auto&& wrapper = tgui::Group::create(
                { deps.sizer.getBaseContainerHeight() * 4, "90%" });
            wrapper->add(WidgetBuilder::createRowButton(
                deps.strings.getString(label),
                std::move(onClick),
                deps.sizer,
                deps.soundPlayer,
                options));
            bottomNavbar->add(wrapper);
            return *this;
        }

        tgui::ChildWindow::Ptr build()
        {
            return modal;
        }

    private:
        detail::ModalBuilderDeps deps;
        tgui::ChildWindow::Ptr modal;
        tgui::GrowHorizontalLayout::Ptr bottomNavbar;
    };

    template<ScopedEnum StringId>
    class [[nodiscard]] ModalBuilderWithOpenModal final
    {
    public:
        ModalBuilderWithOpenModal(
            ModalBuilderDeps&& deps, tgui::ChildWindow::Ptr&& modal)
            : deps(std::move(deps)), modal(std::move(modal))
        {
        }

    public:
        ModalBuilderWithContent<StringId> withContent(
            tgui::Container::Ptr content,
            std::optional<std::string> bottonNavbarClassName = std::nullopt)
        {
            auto&& [body, bottomNavbar] =
                buildLayout(content, bottonNavbarClassName);

            modal->add(body);
            return ModalBuilderWithContent<StringId>(
                std::move(deps), std::move(modal), std::move(bottomNavbar));
        }

    private:
        std::tuple<tgui::Group::Ptr, tgui::GrowHorizontalLayout::Ptr>
        buildLayout(
            tgui::Container::Ptr content,
            std::optional<std::string> bottonNavbarClassName = std::nullopt)
        {
            auto&& container = tgui::Group::create();

            content->setSize(
                { "100%", "100% - ModuleSelectionBottomNavbar.height" });
            container->add(content);

            auto&& bottomNavbar = WidgetBuilder::createContainer<tgui::Panel>(
                { "100%", deps.sizer.getBaseContainerHeight() * 1.5 },
                {
                    .className = bottonNavbarClassName,
                });
            TguiHelper::alignInParent(
                bottomNavbar,
                tgui::HorizontalAlignment::Left,
                tgui::VerticalAlignment::Bottom);
            container->add(bottomNavbar, "ModuleSelectionBottomNavbar");

            auto&& growLayout = tgui::GrowHorizontalLayout::create();

            TguiHelper::alignInParent(
                growLayout,
                tgui::HorizontalAlignment::Right,
                tgui::VerticalAlignment::Center);

            bottomNavbar->add(growLayout);

            return { container, growLayout };
        }

    private:
        detail::ModalBuilderDeps deps;
        tgui::ChildWindow::Ptr modal;
    };
} // namespace detail

template<ScopedEnum StringId>
class [[nodiscard]] ModalBuilder final
{
public:
    ModalBuilder(
        const StringProvider<StringId>& strings,
        const Sizer& sizer,
        GuiAudioInterface& soundPlayer) noexcept
        : deps({ strings, sizer, soundPlayer })
    {
    }

public:
    detail::ModalBuilderWithOpenModal<StringId>
    openModal(StringId title, tgui::Layout2d size)
    {
        auto&& modal = tgui::ChildWindow::create(
            deps.strings.getString(title),
            tgui::ChildWindow::TitleButton::None);

        modal->setSize(size);
        modal->setTitleTextSize(deps.sizer.getBaseFontSize());
        TguiHelper::centerInParent(modal);
        modal->getRenderer()->setTitleBarColor(COLOR_MID_BLUE);
        modal->getRenderer()->setTitleBarHeight(
            static_cast<float>(deps.sizer.getBaseContainerHeight()));
        modal->getRenderer()->setTextSize(deps.sizer.getBaseFontSize());

        return detail::ModalBuilderWithOpenModal<StringId>(
            std::move(deps), std::move(modal));
    }

private:
    detail::ModalBuilderDeps deps;
};
