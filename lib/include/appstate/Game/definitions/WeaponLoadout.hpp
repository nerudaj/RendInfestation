#pragma once

#include "appstate/Game/enums/WeaponModule.hpp"
#include <array>

struct [[nodiscard]] WeaponConfig final
{
    std::array<WeaponModule, 3u> modules = std::array {
        WeaponModule::None,
        WeaponModule::None,
        WeaponModule::None,
    };
};

struct [[nodiscard]] WeaponLoadout final
{
    std::array<WeaponConfig, 2u> weapons = {};
};
