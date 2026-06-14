#pragma once

#include "appstate/Game/definitions/Constants.hpp"
#include "appstate/Game/enums/ProjectileKind.hpp"
#include "appstate/Game/enums/ProjectileTraits.hpp"
#include "audio/SoundId.hpp"
#include <SFML/System/Time.hpp>

struct [[nodiscard]] WeaponProperties final
{
    SoundId::IdType soundId = SoundId::Bullet;
    int baseProjectileDamage = BASE_PROJECTILE_DAMAGE;
    int numShots = 1;
    int spread = 1;
    float kickback = BASE_PROJECTILE_KICKBACK;
    sf::Time fireDelay = BASE_WEAPON_FIRE_DELAY;
    sf::Time projectileLifetime = BASE_PROJECTILE_LIFETIME;
    float projectileSpeed = BASE_PROJECTILE_SPEED;
    float projectileSpeedVariance = 0.f;
    ProjectileTraits projectileTraits = ProjectileTraits::None;
    SkinType projectileSkin = SkinType::SmallBullet;
};
