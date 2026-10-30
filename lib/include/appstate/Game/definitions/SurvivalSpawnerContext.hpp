#pragma once

#include "appstate/Game/definitions/Constants.hpp"
#include "appstate/Game/enums/SurvivalModeState.hpp"
#include <SFML/System/Time.hpp>

struct [[nodiscard]] SurvivalSpawnerContext final
{
    int wave = 0;
    int enemiesInCurrentWave = 0;
    int enemiesSpawnedInCurrentWave = 0;
    int enemiesKilledInCurrentWave = 0;
    sf::Time timeTillNextWave = sf::seconds(5.f);
    sf::Time timeTillNextSpawn = SPAWNER_SPAWN_DELAY;
    sf::Time durationOfTooManyEnemiesConcurrently = sf::seconds(0.f);
    sf::Time durationOfNotEnoughEnemiesConcurrently = sf::seconds(0.f);
    SurvivalModeState state = SurvivalModeState::WaitingForNextWave;
};
