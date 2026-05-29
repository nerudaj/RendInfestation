#pragma once

#include "appstate/Game/definitions/Components.hpp"
#include "appstate/Game/definitions/WeaponProperties.hpp"
#include "appstate/Game/enums/EntityKind.hpp"
#include "appstate/Game/enums/WeaponModule.hpp"
#include <functional>
#include <vector>

class [[nodiscard]] WeaponBuilder final
{
public:
    static std::function<WeaponProperties(WeaponProperties)>
    createWeaponModuleTransformer(WeaponModule module);

    static Weapon
    createWeapon(EntityKind ownerKind, std::vector<WeaponModule> modules);

    static Weapon createMeleeWeapon(EntityKind ownerKind, int damage);

    static Weapon
    createRangedWeapon(EntityKind ownerKind, SkinType bulletSkin, int damage);
};
