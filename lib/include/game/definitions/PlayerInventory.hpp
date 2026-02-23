#pragma once

#include "game/definitions/Weapon.hpp"
#include <array>

struct [[nodiscard]] PlayerInventory final
{
    int health = 0;
    bool activeWeapon = 0;
    std::array<Weapon, 2u> weapons;
};
