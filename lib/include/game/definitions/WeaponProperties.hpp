#pragma once

#include "game/enums/ProjectileKind.hpp"
#include "game/enums/ProjectileTraits.hpp"
#include <SFML/System/Time.hpp>

struct [[nodiscard]] WeaponProperties final
{
    int baseProjectileDamage = 25;
    int numShots = 1;
    int spread = 1;
    float kickback = 0;
    sf::Time fireDelay = sf::seconds(0.5f);
    ProjectileTraits projectileTraits;
    ProjectileKind projectileKind;
};
