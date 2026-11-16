#pragma once

#include "appstate/AppStateChooseBonus.hpp"
#include "appstate/Game/Janitor.hpp"
#include "appstate/Game/definitions/GameMode.hpp"
#include "appstate/Game/definitions/GameScene.hpp"
#include "appstate/Game/definitions/GameTextureAtlas.hpp"
#include "appstate/Game/engine/AiEngine.hpp"
#include "appstate/Game/engine/AnimationEngine.hpp"
#include "appstate/Game/engine/EmptyHealthHandlerEngine.hpp"
#include "appstate/Game/engine/GameRulesEngine.hpp"
#include "appstate/Game/engine/LifetimeEngine.hpp"
#include "appstate/Game/engine/ParticleEngine.hpp"
#include "appstate/Game/engine/PhysicsEngine.hpp"
#include "appstate/Game/engine/RenderingEngine.hpp"
#include "appstate/Game/survival/SurvivalBoonUnlockEvaluator.hpp"
#include "appstate/Game/utility/AudioUtility.hpp"
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
        GameScene& scene,
        const GameModeProperties& gameProps)
        : dgm::AppState(app)
        , dic(dic)
        , atlas(atlas)
        , scene(scene)
        , audioUtility(scene, dic.soundPlayer)
        , aiEngine(scene)
        , gameRulesEngine(
              gameEvents,
              scene,
              atlas,
              dic.input,
              audioUtility,
              dic.strings,
              dic.saveManager,
              gameProps.mapIdx)
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
        , lifetimeEngine(scene, gameEvents, atlas)
        , healthHandlerEngine(scene, gameEvents, audioUtility)
        , boonUnlockEvaluator(dic.saveManager)
        , gameProps(gameProps)
    {
        srand(static_cast<unsigned>(time(nullptr)));
        gameEvents.pushEvent<event::WaveEnded>();
        dic.jukebox.changePlaylist(PLAYLIST.gameSongs);
        aiEngine.setAttackTicketCount(
            gameProps.boons.hardMode ? HARD_ATTACK_TICKET_COUNT
                                     : EASY_ATTACK_TICKET_COUNT);
    }

    ~AppStateGame()
    {
        dic.jukebox.changePlaylist(PLAYLIST.menuSongs);
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
    AudioUtility audioUtility;
    AiEngine aiEngine;
    GameRulesEngine gameRulesEngine;
    AnimationEngine animationEngine;
    PhysicsEngine physicsEngine;
    ParticleEngine particleEngine;
    RenderingEngine renderingEngine;
    LifetimeEngine lifetimeEngine;
    EmptyHealthHandlerEngine healthHandlerEngine;
    Janitor janitor;
    SurvivalBoonUnlockEvaluator boonUnlockEvaluator;
    const GameModeProperties gameProps;
};
