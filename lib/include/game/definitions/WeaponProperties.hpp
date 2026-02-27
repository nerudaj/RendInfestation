#pragma once

#include "game/definitions/Constants.hpp"
#include "game/enums/ProjectileKind.hpp"
#include "game/enums/ProjectileTraits.hpp"
#include <SFML/System/Time.hpp>

struct [[nodiscard]] WeaponProperties final
{
    int baseProjectileDamage = BASE_PROJECTILE_DAMAGE;
    int numShots = 1;
    int spread = 1;
    float kickback = BASE_PROJECTILE_KICKBACK;
    sf::Time fireDelay = BASE_WEAPON_FIRE_DELAY;
    float projectileSpeed = BASE_PROJECTILE_SPEED;
    ProjectileTraits projectileTraits = ProjectileTraits::None;
    ActorSkin projectileSkin = ActorSkin::SmallBullet;
};
