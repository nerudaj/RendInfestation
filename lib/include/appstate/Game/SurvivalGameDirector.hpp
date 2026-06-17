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
    SurvivalGameDirector();

public:
    void update(const sf::Time& time, EventQueue<GameEvent>& eventQueue);

    /// <returns>Score for killing the enemy</returns>
    int markKilledEnemy(const Skin& skin);

    const SurvivalSpawnerContext& getContext() const noexcept
    {
        return context;
    }

    static SurvivalSpawnerContext createInitialContext();

private:
    int getScoreForEnemy(const Skin& skin) const;

    SkinType getEnemyToSpawn() const;

    static int getEnemyCountForWave(int wave);

private:
    SurvivalSpawnerContext context;
};
