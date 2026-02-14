#pragma once

#include "game/definitions/Weapon.hpp"

struct [[nodiscard]] PlayerInventory final
{
    int health = 0;
    Weapon weapon;
};
