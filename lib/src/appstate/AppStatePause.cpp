#include "appstate/AppStatePause.hpp"
#include "appstate/AppStateOptions.hpp"
#include "appstate/AppStateWeaponModification.hpp"
#include "appstate/CommonHandler.hpp"
#include "appstate/Messaging.hpp"
#include "gui/Icon.hpp"

void AppStatePause::input()
{
    CommonHandler::handleInput(app, dic, dic.settings.input);
}

void AppStatePause::update() {}

void AppStatePause::draw()
{
    dic.gui.draw();
    dic.virtualCursor.draw();
}

void AppStatePause::buildLayout()
{
    auto createIconButton = [&](Icon icon, auto&& callback)
    {
        return WidgetBuilder::createTexturedButton(
            dic.resmgr.get<tgui::Texture>(
                uni::format("Icon-{}", std::to_underlying(icon))),
            std::forward<decltype(callback)>(callback),
            dic.soundPlayer);
    };

    dic.gui.rebuildWith(
        dic.guiBuilderFactory.createDefaultLayoutBuiler()
            .withBackgroundImage(dic.resmgr.get<tgui::Texture>("darken.png"))
            .withTitle(
                dic.strings.getString(StringId::PauseTitle), HeadingLevel::H1)
            .withContent(dic.guiBuilderFactory.createButtonListBuilder()
                             .addButton(
                                 StringId::WeaponModification,
                                 [&] { onWeaponModification(); })
                             .addButton(StringId::ExitButton, [&] { onExit(); })
                             .build())
            .withTopLeftButton(
                createIconButton(Icon::Exit, [&] { onBackToMenu(); }))
            .withTopRightButton(
                createIconButton(Icon::Settings, [&] { onOptions(); }))
            .withBottomLeftButton(
                createIconButton(Icon::Play2, [&] { onResume(); }))
            .withNoBottomRightButton()
            .build());

    return;
    dic.gui.rebuildWith(
        dic.guiBuilderFactory.createDefaultLayoutBuiler()
            .withNoBackgroundImage()
            .withTitle(
                dic.strings.getString(StringId::PauseTitle), HeadingLevel::H1)
            .withContent(
                dic.guiBuilderFactory.createButtonListBuilder()
                    .addButton(StringId::Resume, [&] { onResume(); })
                    .addButton(
                        StringId::WeaponModification,
                        [&] { onWeaponModification(); })
                    .addButton(StringId::Options, [&] { onOptions(); })
                    .addButton(StringId::BackToMenu, [&] { onBackToMenu(); })
                    .addButton(StringId::ExitButton, [&] { onExit(); })
                    .build())
            .withNoCornerButtons()
            .build());
}

void AppStatePause::onResume()
{
    app.popState();
}

void AppStatePause::onWeaponModification()
{
    app.pushState<AppStateWeaponModification>(dic, scene);
}

void AppStatePause::onOptions()
{
    app.pushState<AppStateOptions>(dic);
}

void AppStatePause::onBackToMenu()
{
    app.popState(Messaging::serialize<PopIfNotMenu>());
}

void AppStatePause::onExit()
{
    app.exit();
}

void AppStatePause::restoreFocusImpl(const std::string& message)
{
    auto msg = Messaging::deserialize(message);
    if (msg)
    {
        std::visit(
            overloads {
                [&](PopIfNotGame&) { app.popState(message); },
                [&](PopIfNotMenu&) { app.popState(message); },
            },
            *msg);
    }

    buildLayout();
}
