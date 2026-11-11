#pragma once

#include "appstate/Game/SurvivalGameDirector.hpp"
#include "appstate/Game/Types.hpp"
#include "appstate/Game/definitions/CameraZoomHandler.hpp"
#include "appstate/Game/definitions/Components.hpp"
#include "appstate/Game/definitions/Constants.hpp"
#include "appstate/Game/definitions/LightSource.hpp"
#include "appstate/Game/definitions/SurvivalSpawnerContext.hpp"
#include "appstate/Game/definitions/WeaponLoadout.hpp"
#include "appstate/Game/enums/WeaponModule.hpp"
#include <DGM/dgm.hpp>
#include <array>
#include <entt/entt.hpp>
#include <queue>
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
    std::queue<StringId> queuedMessages;
    sf::Time displayTime = sf::Time::Zero;
};

using Ledger = std::set<StringId>;

struct [[nodiscard]] GameStatus final
{
    bool finished = false;
    int score = 0;
    int enemiesKilled = 0;
    int shotsFired = 0;
    int waveBeaten = 0;
    Ledger ledger = {};
};

struct [[nodiscard]] EnemyTag final
{
};

struct [[nodiscard]] PlayerInventory final
{
    WeaponLoadout loadout;
    std::set<WeaponModule> unlockedModules = {};
    int playerMaxHealth = BASE_PLAYER_HEALTH;
    bool emergencyReviveActive = false;
};

struct [[nodiscard]] GameScene final
{
    GameStatus status;
    entt::registry actors;
    entt::entity playerEntity;
    entt::entity playerEntityLower;
    dgm::Mesh levelMesh;
    dgm::Mesh decorationsMesh;
    dgm::Mesh altLevelMesh;
    dgm::Rect levelBounds;
    std::vector<sf::Vector2f> enemySpawns;
    const std::vector<LightSource> lights;
    std::optional<SurvivalGameDirector> survivalGameDirector = std::nullopt;
    HudMessage hudMessage;
    std::optional<InteractionTriggerInventory> interactionTrigger =
        std::nullopt;
    PlayerInventory playerInventory;

    // recomputed each frame, used for progressive cooldowns
    std::map<SkinType, unsigned> skinCounts = {};
    size_t actorWithInputCount = 0;

    void updatePlayerLoadout();
};
