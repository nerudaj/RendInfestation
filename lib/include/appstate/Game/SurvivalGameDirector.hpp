#pragma once

#include "appstate/Game/definitions/Components.hpp"
#include "appstate/Game/definitions/GameEvents.hpp"
#include "appstate/Game/definitions/SurvivalSpawnerContext.hpp"
#include "misc/EventQueue.hpp"
#include <SFML/System/Time.hpp>
#include <optional>

class [[nodiscard]] SurvivalGameDirector final
{
public:
    SurvivalGameDirector(
        EventQueue<GameEvent>& eventQueue,
        SurvivalSpawnerContext& context) noexcept
        : eventQueue(eventQueue), context(context)
    {
    }

public:
    void update(const sf::Time& time);

    int getScoreForEnemy(const Skin& skin) const;

private:
    SkinType getEnemyToSpawn() const;

private:
    EventQueue<GameEvent>& eventQueue;
    SurvivalSpawnerContext& context;
};
