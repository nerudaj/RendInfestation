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
    WeaponCombo startingWeapon = WeaponCombo::None;
};

struct [[nodiscard]] GameModeProperties final
{
    GameMode mode;
    std::string mapName;
    GameBoons boons;
};
