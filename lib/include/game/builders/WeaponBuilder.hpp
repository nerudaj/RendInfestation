#pragma once

#include "game/definitions/Weapon.hpp"
#include "game/definitions/WeaponProperties.hpp"
#include "game/enums/WeaponModule.hpp"
#include <functional>
#include <vector>

class [[nodiscard]] WeaponBuilder final
{
public:
    static std::function<WeaponProperties(WeaponProperties)>
    createWeaponModuleTransformer(WeaponModule module);

    static Weapon createWeapon(const std::vector<WeaponModule>& modules);
};
