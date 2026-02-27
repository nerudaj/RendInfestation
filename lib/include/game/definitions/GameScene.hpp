#pragma once

#include "game/Types.hpp"
#include "game/definitions/Actor.hpp"
#include "game/definitions/DamageMarkerInventory.hpp"
#include "game/definitions/EnemyInventory.hpp"
#include "game/definitions/PlayerInventory.hpp"
#include "game/definitions/ProjectileInventory.hpp"
#include <DGM/dgm.hpp>
#include <vector>

using Inventory = std::variant<
    NpcInventory,
    PlayerInventory,
    ProjectileInventory,
    DamageMarkerInventory>;

struct [[nodiscard]] GameScene final
{
    dgm::DynamicBuffer<Actor, ActorIndexType> actors;
    dgm::DynamicBuffer<Inventory, InventoryIndexType> inventories;
    dgm::Mesh levelMesh;
    dgm::Rect levelBounds;
    sf::Vector2f cameraPosition = { 0.f, 0.f };
    size_t tick = 0;
    std::vector<sf::Vector2f> enemySpawns;
    sf::Time spawnTicker;
    const sf::Time spawnDelay = sf::seconds(1.f);
};
