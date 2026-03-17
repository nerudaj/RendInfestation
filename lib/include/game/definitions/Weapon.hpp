#pragma once

#include "audio/SoundId.hpp"
#include "game/definitions/ProjectileInventory.hpp"
#include "game/enums/ActorSkin.hpp"
#include "game/enums/ProjectileTraits.hpp"

struct [[nodiscard]] Weapon final
{
    SoundId::IdType soundId = SoundId::Bullet;
    sf::Time cooldown = sf::seconds(0.1f);
    sf::Time timeTillFire = sf::seconds(0.f);
    sf::Time projectileLifetime = sf::seconds(0.f);
    float kickback = 0.f;
    float projectileSpeed = 0.f;
    int spread = 1; // degrees
    int numShots = 1;
    SkinType projectileSkin = SkinType::SmallBullet;
    ProjectileInventory defaultProjectileInventory;
};
