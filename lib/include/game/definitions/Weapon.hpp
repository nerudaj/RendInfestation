#pragma once

#include "game/definitions/ProjectileInventory.hpp"
#include "game/enums/ProjectileTraits.hpp"

struct [[nodiscard]] Weapon final
{
    const sf::Time cooldown = sf::seconds(0.6f);
    sf::Time timeTillFire = sf::seconds(0.f);
    float kickback = 100.f;
    int spread = 10; // degrees
    int numShots = 8;
    const ProjectileInventory defaultProjectileInventory;
};
