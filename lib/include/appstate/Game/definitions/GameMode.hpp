#pragma once

#include "appstate/Game/enums/WeaponCombo.hpp"
#include <string>

enum class [[nodiscard]] GameMode
{
    Story,
    Survival,
};

struct [[nodiscard]] GameBoons final
{
    bool wave4start = false;
    bool extraWeapon = false;
    bool revive = false;
    WeaponCombo startingWeapon = WeaponCombo::None;
};

struct [[nodiscard]] GameModeProperties final
{
    GameMode mode;
    size_t mapIdx = 0;
    std::string mapName;
    GameBoons boons;
};
