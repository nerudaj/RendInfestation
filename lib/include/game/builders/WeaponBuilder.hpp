#pragma once

#include "game/definitions/Components.hpp"
#include "game/definitions/WeaponProperties.hpp"
#include "game/enums/EntityKind.hpp"
#include "game/enums/WeaponModule.hpp"
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
