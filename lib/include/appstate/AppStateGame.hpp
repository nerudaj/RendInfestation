#pragma once

#include "appstate/AppStateChooseBonus.hpp"
#include "appstate/Game/Janitor.hpp"
#include "appstate/Game/builders/GameSceneBuilder.hpp"
#include "appstate/Game/builders/GameTextureAtlasBuilder.hpp"
#include "appstate/Game/definitions/GameMode.hpp"
#include "appstate/Game/definitions/GameScene.hpp"
#include "appstate/Game/definitions/GameTextureAtlas.hpp"
#include "appstate/Game/engine/AiEngine.hpp"
#include "appstate/Game/engine/AnimationEngine.hpp"
#include "appstate/Game/engine/GameRulesEngine.hpp"
#include "appstate/Game/engine/ParticleEngine.hpp"
#include "appstate/Game/engine/PhysicsEngine.hpp"
#include "appstate/Game/engine/RenderingEngine.hpp"
#include "misc/DependencyContainer.hpp"
#include "misc/EventQueue.hpp"
#include "settings/AppSettings.hpp"
#include <DGM/dgm.hpp>
#include <SFML/Audio.hpp>
#include <vector>

class [[nodiscard]] AppStateGame : public dgm::AppState
{
public:
    AppStateGame(
        dgm::App& app,
        DependencyContainer& dic,
        const GameModeProperties& gameModeProperties)
        : dgm::AppState(app)
        , dic(dic)
        , atlas(GameTextureAtlasBuilder::createTextureAtlas(
              dic.resmgr, { 1024, 1024 }))
        , scene(GameSceneBuilder::createScene(
              atlas, dic.resmgr, dic.input, gameModeProperties))
        , aiEngine(scene)
        , gameRulesEngine(gameEvents, scene, atlas, dic.input, dic.soundPlayer)
        , animationEngine(scene, gameEvents, atlas)
        , physicsEngine(scene, gameEvents)
        , particleEngine(scene)
        , renderingEngine(
              dic.resmgr,
              scene,
              atlas,
              dic.settings,
              dic.touchController,
              dic.strings)
    {
        srand(static_cast<unsigned>(time(nullptr)));
        gameEvents.pushEvent<event::WaveEnded>();
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
    AiEngine aiEngine;
    GameRulesEngine gameRulesEngine;
    AnimationEngine animationEngine;
    PhysicsEngine physicsEngine;
    ParticleEngine particleEngine;
    RenderingEngine renderingEngine;
    Janitor janitor;
};
