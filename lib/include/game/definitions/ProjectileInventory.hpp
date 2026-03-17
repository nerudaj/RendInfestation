#pragma once

#include "game/definitions/Constants.hpp"
#include "game/enums/ActorKind.hpp"
#include "game/enums/ProjectileTraits.hpp"
#include <SFML/System.hpp>

struct [[nodiscard]] ProjectileInventory final
{
    int damage = 0;
    ProjectileTraits traits = ProjectileTraits::None;
    ActorKind originator = {};
};
