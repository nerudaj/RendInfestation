#include "appstate/AppStateMainMenu.hpp"
#include "appstate/AppStateGame.hpp"
#include "appstate/AppStateOptions.hpp"
#include "appstate/CommonHandler.hpp"
#include "appstate/Game/definitions/GameMode.hpp"
#include "gui/Icon.hpp"
#include "misc/CMakeVars.hpp"
#include "strings/StringProvider.hpp"
#include "types/SemanticTypes.hpp"

AppStateMainMenu::AppStateMainMenu(
    dgm::App& app, DependencyContainer& dic) noexcept
    : dgm::AppState(app), dic(dic)
{
    buildLayout();
    dic.jukebox.play("dstechnician-angry-robot-iii.ogg", "looping"_true);
}

void AppStateMainMenu::input()
{
    CommonHandler::handleInput(
        app,
        dic,
        dic.settings.input,
        CommonHandlerOptions {
            .disableGoBack = true,
        });
}

void AppStateMainMenu::update() {}

void AppStateMainMenu::draw()
{
    dic.gui.draw();
    dic.virtualCursor.draw();
}

void AppStateMainMenu::restoreFocusImpl(const std::string&)
{
    buildLayout();
}

void AppStateMainMenu::buildLayout()
{
    auto& builderFactory = dic.guiBuilderFactory;

    auto createDesktopLayout =
        [&](priv::LayoutBuilderWithBackgroundAndTitle<StringId>& builder)
    {
        return builder
            .withContent(
                builderFactory.createButtonListBuilder()
                    .addButton(StringId::PlayButton, [&] { onPlay(); })
                    .addButton(StringId::SurvivalButton, [&] { onSurvival(); })
                    .addButton(StringId::Options, [&] { onOptions(); })
                    .addButton(
                        StringId::ExitButton,
                        [&] { onExit(); },
                        "MainMenu_Button_Exit")
                    .build())
            .withNoCornerButtons();
    };

    auto createAndroidLayout =
        [&](priv::LayoutBuilderWithBackgroundAndTitle<StringId>& builder)
    {
        return builder
            .withContent(
                builderFactory.createButtonListBuilder()
                    .addButton(StringId::PlayButton, [&] { onPlay(); })
                    .addButton(StringId::SurvivalButton, [&] { onSurvival(); })
                    .build())
            .withNoTopLeftButton()
            .withTopRightButton(
                dic.getIcon(Icon::Settings), [&] { onOptions(); })
            .withNoBottomLeftButton()
            .withNoBottomRightButton();
    };

    auto&& builderWithTitle =
        builderFactory.createDefaultLayoutBuilder()
            .withNoBackground()
            .withTitle(StringId::GameTitle, HeadingLevel::H1);

    dic.gui.rebuildWith(
#ifdef ANDROID
        createAndroidLayout(builderWithTitle)
#else
        createDesktopLayout(builderWithTitle)
#endif
            .build());
}

void AppStateMainMenu::onPlay()
{
    app.pushState<AppStateGame>(
        dic,
        GameModeProperties {
            .mode = GameMode::Story,
            .mapName = "demo-01.json",
        });
}

void AppStateMainMenu::onSurvival()
{
    app.pushState<AppStateGame>(
        dic,
        GameModeProperties {
            .mode = GameMode::Survival,
            .mapName = "survival-02.json",
        });
}

void AppStateMainMenu::onOptions()
{
    app.pushState<AppStateOptions>(dic);
}

void AppStateMainMenu::onExit()
{
    app.exit();
}
