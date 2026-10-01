#include "appstate/AppStateMainMenu.hpp"
#include "appstate/AppStateDimmer.hpp"
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
    dic.jukebox.play(JukeboxMode::Menu);
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

    dic.gui.rebuildWith(
        builderFactory.createDefaultLayoutBuilder()
            .withBackgroundImage(
                dic.resmgr.get<tgui::Texture>("main_screen.png"))
            .withTitle(StringId::GameTitle, HeadingLevel::H1)
            .withContent(
                builderFactory.createButtonListBuilder()
                    .addButton(StringId::SurvivalButton, [&] { onSurvival(); })
                    .addButton(StringId::Options, [&] { onOptions(); })
                    .addButton(
                        StringId::ExitButton,
                        [&] { onExit(); },
                        "MainMenu_Button_Exit")
                    .build())
            .withNoCornerButtons()
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
    // TODO: not perceptible due to long loading times
    AppStateTransitions::applyFadeInTransition<AppStateGame>(
        app,
        FadeInOptions(FadeOptions {
            .duration = sf::seconds(1.f),
        }),
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
