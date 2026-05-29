#include "appstate/Game/builders/WeaponBuilder.hpp"

std::function<WeaponProperties(WeaponProperties)>
WeaponBuilder::createWeaponModuleTransformer(WeaponModule module)
{
    switch (module)
    {
        using enum WeaponModule;
    case None:
        return std::identity {};
    case CadenceBarrel:
        return [](WeaponProperties props)
        {
            props.fireDelay /= 4.f;
            props.kickback += 10.f;
            props.baseProjectileDamage -= props.baseProjectileDamage / 4;
            props.spread *= 2;
            return props;
        };

    case SpreadBarrel_x2:
        return [](WeaponProperties props)
        {
            props.soundId = SoundId::Shotgun;
            props.numShots *= 2;
            // props.baseProjectileDamage /= 2;
            props.kickback += 50.f;
            props.fireDelay *= 1.25f;
            props.spread *= 4;
            return props;
        };

    case SpreadBarrel_x4:
        return [](WeaponProperties props)
        {
            props.soundId = SoundId::Shotgun;
            props.numShots *= 4;
            // props.baseProjectileDamage /= 2;
            props.kickback += 100.f;
            props.fireDelay *= 1.5f;
            props.spread *= 8;
            return props;
        };

    case BigBullet:
        return [](WeaponProperties props)
        {
            props.projectileSpeed /= 2.f;
            props.baseProjectileDamage *= 1.5f;
            props.kickback += 150.f;
            props.fireDelay *= 1.75f;
            props.projectileTraits =
                props.projectileTraits | ProjectileTraits::Big;
            return props;
        };

    case Ricochet:
        return [](WeaponProperties props)
        {
            props.projectileTraits =
                props.projectileTraits | ProjectileTraits::Bouncy;
            return props;
        };

    case PassthruAmmo:
        return [](WeaponProperties props)
        {
            props.baseProjectileDamage /= 2;
            props.projectileTraits =
                props.projectileTraits | ProjectileTraits::Passthru;
            return props;
        };

    case Spikes:
        return [](WeaponProperties props)
        {
            props.projectileSpeed /= 2.f;
            props.kickback += 50.f;
            props.projectileSkin = SkinType::Spikes;
            props.projectileTraits =
                props.projectileTraits | ProjectileTraits::Shrapnels;
            props.spread *= 4;
            props.projectileSpeedVariance = props.projectileSpeed * 0.1f;
            return props;
        };
    case ExplosiveAmmo:
        return [](WeaponProperties props)
        {
            props.projectileTraits =
                props.projectileTraits | ProjectileTraits::Explosive;
            if (props.projectileSkin == SkinType::Spikes)
                props.projectileSkin = SkinType::Landmine;
            props.kickback += 100.f;
            props.fireDelay *= 2.f;
            props.baseProjectileDamage *= 4;
            return props;
        };

    case Splitter:
        return [](WeaponProperties props)
        {
            props.projectileTraits =
                props.projectileTraits | ProjectileTraits::SplitOnHit;
            return props;
        };
    }
}

Weapon WeaponBuilder::createWeapon(EntityKind ownerKind, WeaponConfig config)
{
    std::ranges::sort(config.modules);

    auto properties = WeaponProperties();
    for (auto module : config.modules)
    {
        properties = createWeaponModuleTransformer(module)(properties);
    }

    return Weapon {
        .soundId = properties.soundId,
        .cooldown = properties.fireDelay,
        .projectileLifetime = BASE_PROJECTILE_LIFETIME,
        .kickback = properties.kickback,
        .projectileSpeed = properties.projectileSpeed,
        .projectileSpeedVariance = properties.projectileSpeedVariance,
        .spread = properties.spread,
        .numShots = properties.numShots,
        .projectileSkin = properties.projectileSkin,
        .defaultProjectileInventory =
            ProjectileInventory {
                .damage = properties.baseProjectileDamage,
                .traits = properties.projectileTraits,
                .originator = ownerKind,
            },
    };
}

Weapon WeaponBuilder::createMeleeWeapon(EntityKind ownerKind, int damage)
{
    return Weapon {
        .soundId = SoundId::BigheadAttack,
        .cooldown = sf::seconds(0.5f),
        .projectileLifetime = sf::seconds(0.f),
        .kickback = 20.f,
        .projectileSpeed = 0.f,
        .projectileSkin = SkinType::SmallBullet,
        .defaultProjectileInventory =
            ProjectileInventory {
                .damage = damage,
                .originator = ownerKind,
            },
    };
}

Weapon WeaponBuilder::createRangedWeapon(
    EntityKind ownerKind, SkinType bulletSkin, int damage)
{
    return Weapon {
        .soundId = SoundId::BehemothAttack,
        .cooldown = sf::seconds(1.5f),
        .projectileLifetime = BASE_PROJECTILE_LIFETIME,
        .kickback = 10.f,
        .projectileSpeed = BASE_PROJECTILE_SPEED / 3.f,
        .projectileSkin = bulletSkin,
        .defaultProjectileInventory =
            ProjectileInventory {
                .damage = damage,
                .originator = ownerKind,
            },
    };
}
