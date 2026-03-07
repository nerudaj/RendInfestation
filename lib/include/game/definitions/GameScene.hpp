#pragma once

#include "game/Types.hpp"
#include "game/definitions/Actor.hpp"
#include "game/definitions/DamageMarkerInventory.hpp"
#include "game/definitions/EnemyInventory.hpp"
#include "game/definitions/PlayerInventory.hpp"
#include "game/definitions/ProjectileInventory.hpp"
#include <DGM/dgm.hpp>
#include <entt/entt.hpp>
#include <vector>

using Inventory = std::variant<
    NpcInventory,
    PlayerInventory,
    ProjectileInventory,
    DamageMarkerInventory>;

struct [[nodiscard]] GameScene final
{
    entt::registry actors;
    entt::entity playerEntity;
    dgm::Mesh levelMesh;
    dgm::Mesh altLevelMesh;
    dgm::Rect levelBounds;
    size_t tick = 0;
    std::vector<sf::Vector2f> enemySpawns;
    sf::Time spawnTicker;
    const sf::Time spawnDelay = sf::seconds(1.f);
};
