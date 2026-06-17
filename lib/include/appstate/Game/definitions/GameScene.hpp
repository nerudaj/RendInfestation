#pragma once

#include "appstate/Game/SurvivalGameDirector.hpp"
#include "appstate/Game/Types.hpp"
#include "appstate/Game/definitions/Components.hpp"
#include "appstate/Game/definitions/Constants.hpp"
#include "appstate/Game/definitions/LightSource.hpp"
#include "appstate/Game/definitions/SurvivalSpawnerContext.hpp"
#include "appstate/Game/definitions/WeaponLoadout.hpp"
#include "appstate/Game/enums/WeaponModule.hpp"
#include <DGM/dgm.hpp>
#include <array>
#include <entt/entt.hpp>
#include <set>
#include <vector>

struct [[nodiscard]] TimedScript final
{
    sf::Time timer = {};
    std::function<void()> callback;
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
    std::set<WeaponModule> unlockedModules = {};
    std::optional<SurvivalGameDirector> survivalGameDirector = std::nullopt;
    HudMessage hudMessage;
    std::optional<InteractionTriggerInventory> interactionTrigger =
        std::nullopt;
    int playerMaxHealth = BASE_PLAYER_HEALTH;

    void updatePlayerLoadout();
};
