#include "appstate/AppStateMainMenu.hpp"
#include "appstate/AppStateGame.hpp"
#include "appstate/AppStateOptions.hpp"
#include "appstate/CommonHandler.hpp"
#include "misc/CMakeVars.hpp"
#include "strings/StringProvider.hpp"
#include "types/SemanticTypes.hpp"

AppStateMainMenu::AppStateMainMenu(
    dgm::App& app, DependencyContainer& dic) noexcept
    : dgm::AppState(app), dic(dic)
{
    buildLayout();
    dic.jukebox.play("vampire_killer.wav", "looping"_true);
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
    dic.gui.rebuildWith(
        dic.guiBuilderFactory.createDefaultLayoutBuiler()
            .withBackgroundImage(
                dic.resmgr.get<sf::Texture>("placeholder-background.png"))
            .withTitle(CMakeVars::TITLE, HeadingLevel::H1)
            .withContent(dic.guiBuilderFactory.createButtonListBuilder()
                             .addButton(StringId::PlayButton, [&] { onPlay(); })
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
    app.pushState<AppStateGame>(dic);
}

void AppStateMainMenu::onOptions()
{
    app.pushState<AppStateOptions>(dic);
}

void AppStateMainMenu::onExit()
{
    app.exit();
}
