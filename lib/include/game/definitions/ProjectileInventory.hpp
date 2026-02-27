#pragma once

#include "game/definitions/Constants.hpp"
#include "game/enums/ProjectileTraits.hpp"
#include <SFML/System.hpp>

struct [[nodiscard]] ProjectileInventory final
{
    int damage = 0;
    ProjectileTraits traits = ProjectileTraits::None;
    sf::Time lifetime = BASE_PROJECTILE_LIFETIME;
};
