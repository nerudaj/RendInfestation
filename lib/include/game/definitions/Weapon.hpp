#pragma once

#include "game/definitions/ProjectileInventory.hpp"
#include "game/enums/ActorSkin.hpp"
#include "game/enums/ProjectileTraits.hpp"

struct [[nodiscard]] Weapon final
{
    const sf::Time cooldown = sf::seconds(0.1f);
    sf::Time timeTillFire = sf::seconds(0.f);
    float kickback = 0.f;
    int spread = 0; // degrees
    int numShots = 0;
    float projectileSpeed = 0.f;
    ActorSkin projectileSkin = ActorSkin::SmallBullet;
    const ProjectileInventory defaultProjectileInventory;
};
