#pragma once

#include "game/definitions/Weapon.hpp"
#include <array>

struct [[nodiscard]] PlayerInventory final
{
    bool activeWeapon = 0;
    std::array<Weapon, 2u> weapons;
};
