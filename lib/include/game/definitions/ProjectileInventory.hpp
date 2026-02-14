#pragma once

#include <SFML/System.hpp>
#include "game/enums/ProjectileTraits.hpp"

struct [[nodiscard]] ProjectileInventory final
{
    sf::Time lifetime;
    int damage = 0;
    ProjectileTraits = ProjectileTraits::None;
};