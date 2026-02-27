#include "game/builders/WeaponBuilder.hpp"

std::function<WeaponProperties(WeaponProperties)>
WeaponBuilder::createWeaponModuleTransformer(WeaponModule module)
{
    switch (module)
    {
        using enum WeaponModule;
    case None:
        return std::identity {};
    case SpreadBarrel:
        return [](WeaponProperties props)
        {
            props.numShots = 4;
            props.baseProjectileDamage /= 2;
            props.kickback += 100.f;
            props.fireDelay *= 1.2f;
            props.spread *= 4;
            return props;
        };
    case CadenceBarrel:
        return [](WeaponProperties props)
        {
            props.fireDelay /= 5.f;
            props.kickback += 10.f;
            props.baseProjectileDamage -= props.baseProjectileDamage / 10;
            props.spread *= 2;
            return props;
        };
    case ExplosiveAmmo:
        return [](WeaponProperties props)
        {
            props.projectileTraits = ProjectileTraits::Explosive;
            if (props.projectileKind == ProjectileKind::Spike)
                props.projectileKind = ProjectileKind::Landmine;
            props.kickback += 150.f;
            props.fireDelay *= 1.5f;
            props.baseProjectileDamage *= 10;
            return props;
        };
    case Ricochet:
        return [](WeaponProperties props)
        {
            props.projectileTraits = ProjectileTraits::Bouncy;
            return props;
        };
    }
}

Weapon WeaponBuilder::createWeapon(const std::vector<WeaponModule>& modules)
{
    auto properties = WeaponProperties();
    for (auto module : modules)
    {
        properties = createWeaponModuleTransformer(module)(properties);
    }

    return Weapon {
        .cooldown = properties.fireDelay,
        .kickback = properties.kickback,
        .spread = properties.spread,
        .numShots = properties.numShots,
        .defaultProjectileInventory =
            ProjectileInventory {
                .damage = properties.baseProjectileDamage,
                .traits = properties.projectileTraits,

            },
    };
}
