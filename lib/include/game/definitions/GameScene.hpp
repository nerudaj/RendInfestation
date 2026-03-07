#pragma once

#include "game/Types.hpp"
#include "game/definitions/Actor.hpp"
#include "game/definitions/DamageMarkerInventory.hpp"
#include "game/definitions/EnemyInventory.hpp"
#include "game/definitions/PlayerInventory.hpp"
#include "game/definitions/ProjectileInventory.hpp"
#include "game/enums/WeaponModule.hpp"
#include <DGM/dgm.hpp>
#include <array>
#include <entt/entt.hpp>
#include <vector>

using Inventory = std::variant<
    NpcInventory,
    PlayerInventory,
    ProjectileInventory,
    DamageMarkerInventory>;

struct [[nodiscard]] WeaponLoadout final
{
    std::array<WeaponModule, 3> weapon1Modules = { WeaponModule::None,
                                                   WeaponModule::None,
                                                   WeaponModule::None };
    std::array<WeaponModule, 3> weapon2Modules = { WeaponModule::None,
                                                   WeaponModule::None,
                                                   WeaponModule::None };
};

struct [[nodiscard]] LightSource final
{
    sf::Vector2f position;
    size_t spriteId;
    sf::Color color;
};

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
    const std::vector<LightSource> lights;
    WeaponLoadout loadout;
};
