#pragma once

#include "game/definitions/Weapon.hpp"
#include "game/definitions/WeaponProperties.hpp"
#include "game/enums/ActorKind.hpp"
#include "game/enums/WeaponModule.hpp"
#include <functional>
#include <vector>

class [[nodiscard]] WeaponBuilder final
{
public:
    static std::function<WeaponProperties(WeaponProperties)>
    createWeaponModuleTransformer(WeaponModule module);

    static Weapon
    createWeapon(ActorKind ownerKind, std::vector<WeaponModule> modules);

    static Weapon createMeleeWeapon(ActorKind ownerKind, int damage);
};
