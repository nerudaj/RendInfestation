#pragma once

#include "appstate/Game/Types.hpp"
#include "appstate/Game/definitions/Components.hpp"
#include "appstate/Game/definitions/Constants.hpp"
#include "appstate/Game/enums/WeaponModule.hpp"
#include <DGM/dgm.hpp>
#include <array>
#include <entt/entt.hpp>
#include <set>
#include <vector>

struct [[nodiscard]] WeaponLoadout final
{
    std::array<WeaponModule, 3> weapon1Modules = { WeaponModule::None,
                                                   WeaponModule::None,
                                                   WeaponModule::None };
    std::array<WeaponModule, 3> weapon2Modules = { WeaponModule::None,
                                                   WeaponModule::None,
                                                   WeaponModule::None };
    std::set<WeaponModule> unlockedModules = {};
};

struct [[nodiscard]] LightSource final
{
    sf::Vector2f position;
    size_t spriteId;
    sf::Color color;
};

enum class SurvivalModeState
{
    WaitingForNextWave,
    SpawningEnemies,
    WaitingForEnemiesToDie,
};

struct [[nodiscard]] SurvivalSpawnerContext final
{
    int wave = -1;
    int enemiesInCurrentWave = 10;
    int enemiesSpawnedInCurrentWave = 0;
    int enemiesKilledInCurrentWave = 0;
    sf::Time timeTillNextWave = sf::seconds(5.f);
    sf::Time timeTillNextSpawn = SPAWNER_SPAWN_DELAY;
    SurvivalModeState state = SurvivalModeState::WaitingForNextWave;
};

struct [[nodiscard]] HudMessage final
{
    std::string text;
    sf::Time displayTime = sf::Time::Zero;
};

struct [[nodiscard]] GameStatus final
{
    bool finished = false;
    int score = 0;
};

struct [[nodiscard]] GameScene final
{
    GameStatus status;
    entt::registry actors;
    entt::entity playerEntity;
    dgm::Mesh levelMesh;
    dgm::Mesh decorationsMesh;
    dgm::Mesh altLevelMesh;
    dgm::Rect levelBounds;
    size_t tick = 0;
    std::vector<sf::Vector2f> enemySpawns;
    sf::Time spawnTicker;
    const sf::Time spawnDelay = sf::seconds(1.f);
    const std::vector<LightSource> lights;
    WeaponLoadout loadout;
    SurvivalSpawnerContext survivalSpawnerContext;
    HudMessage hudMessage;
};
