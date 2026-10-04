#include "appstate/AppStateLoading.hpp"
#include "appstate/AppStateDimmer.hpp"
#include "appstate/AppStateGame.hpp"
#include "appstate/CommonHandler.hpp"
#include "appstate/Game/builders/GameSceneBuilder.hpp"
#include "appstate/Game/builders/GameTextureAtlasBuilder.hpp"

AppStateLoading::AppStateLoading(
    dgm::App& app,
    DependencyContainer& dic,
    const GameModeProperties& gameProps)
    : dgm::AppState(app), dic(dic), gameProps(gameProps)
{
    buildLayout();
}

void AppStateLoading::input()
{
    CommonHandler::handleInput(
        app,
        dic,
        dic.settings.input,
        CommonHandlerOptions {
            .disableGoBack = true,
        });
}

void AppStateLoading::update()
{
    if (firstPass)
    {
        firstPass = false;
        return;
    }

    if (!textureAtlas)
    {
        textureAtlas.emplace(GameTextureAtlasBuilder::createTextureAtlas(
            dic.resmgr, { 2048, 2048 }));
        return;
    }

    if (!gameScene)
    {
        gameScene.emplace(GameSceneBuilder::createScene(
            *textureAtlas, dic.resmgr, dic.input, gameProps));
        return;
    }

    AppStateTransitions::applyFadeInTransition<AppStateGame>(
        app,
        FadeInOptions(FadeOptions { .duration = sf::seconds(1.f) }),
        dic,
        *textureAtlas,
        *gameScene);
}

void AppStateLoading::draw()
{
    dic.gui.draw();
    dic.virtualCursor.draw();
}

void AppStateLoading::restoreFocusImpl(const std::string& message)
{
    app.popState(message);
}

void AppStateLoading::buildLayout()
{
    dic.gui.rebuildWith(dic.guiBuilderFactory.createSimpleLayoutBuilder()
                            .withNoBackground()
                            .withPlainTitle(StringId::Loading, HeadingLevel::H1)
                            .withContent(tgui::Group::create())
                            .withNoBottomButtons()
                            .build());
}
