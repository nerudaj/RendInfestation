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
    // dgm::DynamicBuffer<Actor, ActorIndexType> actors;
    // dgm::DynamicBuffer<Inventory, InventoryIndexType> inventories;
    dgm::Mesh levelMesh;
    dgm::Mesh altLevelMesh;
    dgm::Rect levelBounds;
    // sf::Vector2f cameraPosition = { 0.f, 0.f };
    size_t tick = 0;
    std::vector<sf::Vector2f> enemySpawns;
    sf::Time spawnTicker;
    const sf::Time spawnDelay = sf::seconds(1.f);
};

/*
template<class InventoryT>
static std::tuple<Actor&, InventoryT&>
getActorAndInventory(GameScene& scene, ActorIndexType actorIdx)
{
    assert(scene.actors.isIndexValid(actorIdx));
    auto& actor = scene.actors[actorIdx];
    assert(actor.inventoryIdx);
    assert(scene.inventories.isIndexValid(*actor.inventoryIdx));
    assert(std::holds_alternative<InventoryT>(
        scene.inventories[*actor.inventoryIdx]));
    return {
        actor,
        std::get<InventoryT>(scene.inventories[*actor.inventoryIdx]),
    };
}
*/
