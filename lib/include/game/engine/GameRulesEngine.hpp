#pragma once

#include "game/definitions/GameEvents.hpp"
#include "game/definitions/GameScene.hpp"
#include "game/definitions/GameTextureAtlas.hpp"
#include "input/Input.hpp"
#include "misc/EventQueue.hpp"

class [[nodiscard]] GameRulesEngine final
{
public:
    GameRulesEngine(
        EventQueue<GameEvent>& gameEventQueue,
        GameScene& scene,
        const GameTextureAtlas& atlas,
        Input& input) noexcept
        : gameEventQueue(gameEventQueue)
        , scene(scene)
        , atlas(atlas)
        , input(input)
    {
    }

    GameRulesEngine(GameRulesEngine&&) = delete;
    GameRulesEngine(const GameRulesEngine&) = delete;

public:
    void operator()(const DummyGameEvent&) {}

public:
    void update(const dgm::Time& time);

    void updatePlayer(
        Actor& actor, PlayerInventory& inventory, const dgm::Time& time);

    void updateProjectile(
        Actor& actor, ProjectileInventory& inventory, const dgm::Time& time);

private:
    EventQueue<GameEvent>& gameEventQueue;
    GameScene& scene;
    const GameTextureAtlas& atlas;
    Input& input;
};
