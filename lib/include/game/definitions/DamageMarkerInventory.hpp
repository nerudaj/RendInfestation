#pragma once

#include "game/enums/ActorKind.hpp"

struct [[nodiscard]] DamageMarkerInventory final
{
    ActorKind originator;
    int damage;
};
