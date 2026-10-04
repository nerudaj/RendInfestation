#pragma once

#include "appstate/AppStateChooseBonus.hpp"
#include "appstate/Game/Janitor.hpp"
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
        const GameTextureAtlas& atlas,
        GameScene& scene)
        : dgm::AppState(app)
        , dic(dic)
        , atlas(atlas)
        , scene(scene)
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
        dic.jukebox.play(JukeboxMode::Game);
    }

    ~AppStateGame()
    {
        dic.jukebox.play(JukeboxMode::Menu);
    }

public:
    void input() override;

    void update() override;

    void draw() override;

private:
    void restoreFocusImpl(const std::string& msg) override;

private:
    DependencyContainer& dic;
    const GameTextureAtlas& atlas;
    GameScene& scene;
    EventQueue<GameEvent> gameEvents;
    AiEngine aiEngine;
    GameRulesEngine gameRulesEngine;
    AnimationEngine animationEngine;
    PhysicsEngine physicsEngine;
    ParticleEngine particleEngine;
    RenderingEngine renderingEngine;
    Janitor janitor;
};
