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
        EventQueue<GameEvent>& eventQueue,
        GameScene& scene,
        const GameTextureAtlas& atlas,
        Input& input) noexcept
        : eventQueue(eventQueue), scene(scene), atlas(atlas), input(input)
    {
    }

    GameRulesEngine(GameRulesEngine&&) = delete;
    GameRulesEngine(const GameRulesEngine&) = delete;

public:
    void operator()(const event::PlayerWantsToFire& e);

    void operator()(const auto&) {}

public:
    void update(const dgm::Time& time);

    void updatePlayer(
        Actor& actor, PlayerInventory& inventory, const dgm::Time& time);

    void updateProjectile(
        Actor& actor, ProjectileInventory& inventory, const dgm::Time& time);

private:
    EventQueue<GameEvent>& eventQueue;
    GameScene& scene;
    const GameTextureAtlas& atlas;
    Input& input;
};
