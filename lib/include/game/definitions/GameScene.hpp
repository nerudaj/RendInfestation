#pragma once

#include "game/definitions/Actor.hpp"
#include "game/definitions/EnemyInventory.hpp"
#include "game/definitions/PlayerInventory.hpp"
#include "game/definitions/ProjectileInventory.hpp"
#include <DGM/dgm.hpp>
#include <vector>

using Inventory =
    std::variant<NpcInventory, PlayerInventory, ProjectileInventory>;

struct [[nodiscard]] GameScene final
{
    dgm::SpatialBuffer<Actor> actors;
    dgm::DynamicBuffer<Inventory> inventories;
    dgm::Mesh levelMesh;
    sf::Vector2f cameraPosition = { 0.f, 0.f };
    size_t tick = 0;
};
