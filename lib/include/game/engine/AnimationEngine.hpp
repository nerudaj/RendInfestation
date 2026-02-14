#pragma once

#include "game/definitions/GameEvents.hpp"
#include "game/definitions/GameScene.hpp"
#include "misc/EventQueue.hpp"

class [[nodiscard]] AnimationEngine final
{
public:
    AnimationEngine(GameScene& scene, EventQueue<GameEvent>& eventQueue)
        : scene(scene), eventQueue(eventQueue)
    {
    }

public:
    void operator()(const event::PlayerFiredWeapon& e);

    void operator()(const event::ProjectileHitSomething& e);

    void operator()(const auto&) {}

public:
    void update(const dgm::Time& time);

private:
    GameScene& scene;
    EventQueue<GameEvent>& eventQueue;
};
