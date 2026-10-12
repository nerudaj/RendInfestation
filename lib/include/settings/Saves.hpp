#pragma once

#include "appstate/Game/enums/WeaponCombo.hpp"
#include "nlohmann/json.hpp"
#include <set>

struct [[nodiscard]] BoonUnlocks final
{
    bool wave4start = true;
    bool extraWeapon = true;
    std::set<WeaponCombo> weaponCombos = { WeaponCombo::None,
                                           WeaponCombo::Random };
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    BoonUnlocks, wave4start, extraWeapon, weaponCombos);

struct [[nodiscard]] Saves final
{
    int version = 0;
    BoonUnlocks unlockedBoons;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Saves, version, unlockedBoons);
