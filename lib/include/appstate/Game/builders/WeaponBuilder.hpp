#pragma once

#include "appstate/Game/definitions/Components.hpp"
#include "appstate/Game/definitions/WeaponLoadout.hpp"
#include "appstate/Game/definitions/WeaponProperties.hpp"
#include "appstate/Game/enums/EnemyWeaponKind.hpp"
#include "appstate/Game/enums/EntityKind.hpp"
#include "appstate/Game/enums/WeaponModule.hpp"
#include <functional>
#include <vector>

class [[nodiscard]] WeaponBuilder final
{
private:
    static std::function<WeaponProperties(WeaponProperties)>
    createWeaponModuleTransformer(WeaponModule module);

public:
    static Weapon createWeapon(EntityKind ownerKind, WeaponConfig config);

    static Weapon createEnemyWeapon(int damage, EnemyWeaponKind weaponKind);
};
