#pragma once

#include "game/definitions/GameEvents.hpp"
#include "game/definitions/GameScene.hpp"
#include "input/Input.hpp"
#include "misc/EventQueue.hpp"

class [[nodiscard]] GameRulesEngine final
{
public:
    GameRulesEngine(
        EventQueue<GameEvent>& gameEventQueue,
        GameScene& scene,
        Input& input) noexcept
        : gameEventQueue(gameEventQueue), scene(scene), input(input)
    {
    }

    GameRulesEngine(GameRulesEngine&&) = delete;
    GameRulesEngine(const GameRulesEngine&) = delete;

public:
    void operator()(const DummyGameEvent&) {}

public:
    void update(const dgm::Time& time);

private:
    void updateDummyAnimation(
        dgm::Animation& animation,
        const sf::Vector2f& forward,
        const dgm::Time& time);

    void updateDummy(DummyEntity& dummy, const float deltaTime);

private:
    EventQueue<GameEvent>& gameEventQueue;
    GameScene& scene;
    Input& input;
};
