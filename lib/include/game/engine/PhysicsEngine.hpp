#pragma once

#include "game/definitions/GameEvents.hpp"
#include "game/definitions/GameScene.hpp"
#include "misc/EventQueue.hpp"

class [[nodiscard]] PhysicsEngine final
{
public:
    PhysicsEngine(GameScene& scene, EventQueue<GameEvent>& queue)
        : scene(scene), eventQueue(queue)
    {
    }

public:
    void operator()(const auto&) {}

public:
    void update(const dgm::Time& time);

private:
    GameScene& scene;
    EventQueue<GameEvent>& eventQueue;
};
