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

struct [[nodiscard]] ModalBuilderOptions final
{
    tgui::Layout2d modalSize = { "100%", "100%" };
    tgui::Color titleBarColor = tgui::Color::Black;
    bool bodyScrollable = false;
    tgui::Padding bodyPadding = {};
    std::optional<std::string> bodyPanelClass = std::nullopt;
    std::optional<std::string> navbarPanelClass = std::nullopt;
};

namespace detail
{
    struct [[nodiscard]] ModalBuilderDeps final
    {
        const StringProvider<StringId>& strings;
        const Sizer& sizer;
        GuiAudioInterface& soundPlayer;
    };

    struct [[nodiscard]] ModalBuilderContext final
    {
        ModalBuilderDeps deps;
        ModalBuilderOptions options;
        tgui::ChildWindow::Ptr modal;
        tgui::GrowHorizontalLayout::Ptr bottomNavbar;
    };

    template<ScopedEnum StringId>
    class [[nodiscard]] ModalBuilderWithContent final
    {
    public:
        ModalBuilderWithContent(ModalBuilderContext&& context)
            : context(std::move(context))
        {
        }

    public:
        ModalBuilderWithContent<StringId>& withBottomButton(
            StringId label,
            std::function<void(void)> onClick,
            WidgetOptions options = {})
        {
            auto&& wrapper = tgui::Group::create(
                { context.deps.sizer.getBaseContainerHeight() * 4, "100%" });

            auto&& button = WidgetBuilder::createRowButton(
                context.deps.strings.getString(label),
                std::move(onClick),
                context.deps.sizer,
                context.deps.soundPlayer,
                options);

            button->setHeight("80%");

            wrapper->add(button);
            context.bottomNavbar->add(wrapper);
            return *this;
        }

        tgui::ChildWindow::Ptr build()
        {
            return context.modal;
        }

    private:
        ModalBuilderContext context;
    };

    template<ScopedEnum StringId>
    class [[nodiscard]] ModalBuilderWithOpenModal final
    {
    public:
        ModalBuilderWithOpenModal(ModalBuilderContext&& context)
            : context(std::move(context))
        {
        }

    public:
        ModalBuilderWithContent<StringId>
        withContent(tgui::Container::Ptr content)
        {
            auto&& [body, bottomNavbar] = buildLayout(content);

            context.modal->add(body);
            context.bottomNavbar = std::move(bottomNavbar);

            return ModalBuilderWithContent<StringId>(std::move(context));
        }

    private:
        std::tuple<tgui::Container::Ptr, tgui::GrowHorizontalLayout::Ptr>
        buildLayout(tgui::Container::Ptr content)
        {
            auto&& container = tgui::Group::create();

            auto&& body = [&]() -> tgui::Panel::Ptr
            {
                if (context.options.bodyScrollable)
                {
                    auto&& panel =
                        WidgetBuilder::createContainer<tgui::ScrollablePanel>(
                            { "100%",
                              "100% - ModuleSelectionBottomNavbar.height" },
                            context.deps.sizer,
                            WidgetOptions {
                                .className = context.options.bodyPanelClass,
                            });
                    panel->getRenderer()->setScrollbarWidth(
                        context.deps.sizer.getBaseFontSize());
                    return panel;
                }

                return WidgetBuilder::createContainer<tgui::Panel>(
                    { "100%", "100% - ModuleSelectionBottomNavbar.height" },
                    context.deps.sizer,
                    WidgetOptions {
                        .className = context.options.bodyPanelClass,
                    });
            }();
            body->getRenderer()->setPadding(context.options.bodyPadding);
            body->add(content);

            auto&& bottomNavbar = WidgetBuilder::createContainer<tgui::Panel>(
                { "100%", context.deps.sizer.getBaseContainerHeight() * 1.5 },
                context.deps.sizer,
                {
                    .className = context.options.navbarPanelClass,
                });
            TguiHelper::alignInParent(
                bottomNavbar,
                tgui::HorizontalAlignment::Left,
                tgui::VerticalAlignment::Bottom);

            auto&& growLayout = tgui::GrowHorizontalLayout::create();

            TguiHelper::alignInParent(
                growLayout,
                tgui::HorizontalAlignment::Right,
                tgui::VerticalAlignment::Center);

            bottomNavbar->add(growLayout);

            container->add(body);
            container->add(bottomNavbar, "ModuleSelectionBottomNavbar");

            return { container, growLayout };
        }

    private:
        ModalBuilderContext context;
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
    openModal(StringId title, const ModalBuilderOptions& options)
    {
        auto&& modal = tgui::ChildWindow::create(
            deps.strings.getString(title),
            tgui::ChildWindow::TitleButton::None);

        modal->setSize(options.modalSize);
        modal->setTitleTextSize(deps.sizer.getBaseFontSize());
        TguiHelper::centerInParent(modal);
        modal->getRenderer()->setTitleBarColor(options.titleBarColor);
        modal->getRenderer()->setTitleBarHeight(
            static_cast<float>(deps.sizer.getBaseContainerHeight()));
        modal->getRenderer()->setTextSize(deps.sizer.getBaseFontSize());

        return detail::ModalBuilderWithOpenModal<StringId>(
            detail::ModalBuilderContext {
                .deps = std::move(deps),
                .options = options,
                .modal = std::move(modal),
            });
    }

private:
    detail::ModalBuilderDeps deps;
};
