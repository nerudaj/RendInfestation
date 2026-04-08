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
    auto& builderFactory = dic.guiBuilderFactory;

    auto createDesktopLayout =
        [&](priv::LayoutBuilderWithBackgroundAndTitle& builder)
    {
        return builder
            .withContent(
                builderFactory.createButtonListBuilder()
                    .addButton(StringId::Resume, [&] { onResume(); })
                    .addButton(
                        StringId::WeaponModification,
                        [&] { onWeaponModification(); })
                    .addButton(StringId::Options, [&] { onOptions(); })
                    .addButton(StringId::BackToMenu, [&] { onBackToMenu(); })
                    .addButton(StringId::ExitButton, [&] { onExit(); })
                    .build())
            .withNoCornerButtons();
    };

    auto createAndroidLayout =
        [&](priv::LayoutBuilderWithBackgroundAndTitle& builder)
    {
        return builder
            .withContent(builderFactory.createButtonListBuilder()
                             .addButton(
                                 StringId::WeaponModification,
                                 [&] { onWeaponModification(); })
                             .addButton(StringId::ExitButton, [&] { onExit(); })
                             .build())
            .withTopLeftButton(builderFactory.createIconButton(
                Icon::Exit, [&] { onBackToMenu(); }))
            .withTopRightButton(builderFactory.createIconButton(
                Icon::Settings, [&] { onOptions(); }))
            .withBottomLeftButton(builderFactory.createIconButton(
                Icon::Play2, [&] { onResume(); }))
            .withNoBottomRightButton();
    };

    auto&& builderWithTitle =
        builderFactory.createDefaultLayoutBuiler()
            .withBackgroundImage(dic.resmgr.get<tgui::Texture>("darken.png"))
            .withTitle(
                dic.strings.getString(StringId::PauseTitle), HeadingLevel::H1);

    dic.gui.rebuildWith(
#ifdef ANDROID
        createAndroidLayout(builderWithTitle)
#else
        createDesktopLayout(builderWithTitle)
#endif
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
