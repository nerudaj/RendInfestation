#pragma once

#include "game/builders/GameSceneBuilder.hpp"
#include "game/builders/GameTextureAtlasBuilder.hpp"
#include "game/definitions/GameScene.hpp"
#include "game/definitions/GameTextureAtlas.hpp"
#include "game/engine/GameRulesEngine.hpp"
#include "game/engine/RenderingEngine.hpp"
#include "misc/DependencyContainer.hpp"
#include "misc/EventQueue.hpp"
#include "settings/AppSettings.hpp"
#include <DGM/dgm.hpp>
#include <SFML/Audio.hpp>
#include <vector>

class [[nodiscard]] AppStateGame : public dgm::AppState
{
public:
    AppStateGame(dgm::App& app, DependencyContainer& dic)
        : dgm::AppState(
              app,
              dgm::AppStateConfig {
                  .clearColor = sf::Color { 0xff, 0xaa, 0x88 },
              })
        , dic(dic)
        , atlas(GameTextureAtlasBuilder::createTextureAtlas(
              dic.resmgr, { 1024, 1024 }))
        , scene(GameSceneBuilder::createScene(atlas, dic.resmgr))
        , gameRulesEngine(gameEvents, scene, dic.input)
        , renderingEngine(
              dic.resmgr, scene, atlas, dic.settings, dic.touchController)
    {
    }

public:
    void input() override;

    void update() override;

    void draw() override;

private:
    void restoreFocusImpl(const std::string& msg) override;

private:
    DependencyContainer& dic;
    GameTextureAtlas atlas;
    GameScene scene;
    EventQueue<GameEvent> gameEvents;
    GameRulesEngine gameRulesEngine;
    RenderingEngine renderingEngine;
};
