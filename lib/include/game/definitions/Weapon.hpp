#pragma once

#include "game/definitions/ProjectileInventory.hpp"
#include "game/enums/ProjectileTraits.hpp"

struct [[nodiscard]] Weapon final
{
    const sf::Time cooldown = sf::seconds(0.6f);
    sf::Time timeTillFire = sf::seconds(0.f);
    float kickback = 100.f;
    int spread = 10; // degrees
    int numShots = 8;
    const ProjectileInventory defaultProjectileInventory;
};

enum class [[nodiscard]] AmmoType
{
    RegularAmmo,
    ExplosiveAmmo,
    BouncyAmmo,
    Max,
};

enum class [[nodiscard]] BarrelType
{
    SpreadBarrel,
    PrecisionBarrel,
    CadenceBarrel,
    Max,
};

class [[nodiscard]] WeaponBuilder final
{
public:
    static Weapon createRandomWeapon()
    {
        return createWeapon(
            static_cast<BarrelType>(
                rand() % std::to_underlying(BarrelType::Max)),
            static_cast<AmmoType>(rand() % std::to_underlying(AmmoType::Max)));
    }

    static Weapon createWeapon(BarrelType barrel, AmmoType ammo)
    {
        return Weapon {
            .cooldown = barrel == BarrelType::CadenceBarrel ? sf::seconds(0.1f)
                                                            : sf::seconds(0.6f),
            .kickback = barrel == BarrelType::CadenceBarrel ? 100.f : 200.f,
            .spread = barrel == BarrelType::SpreadBarrel    ? 12
                      : barrel == BarrelType::CadenceBarrel ? 2
                                                            : 0,
            .numShots = barrel == BarrelType::SpreadBarrel ? 6 : 1,
            .defaultProjectileInventory =
                ProjectileInventory {
                    .damage = ammo == AmmoType::ExplosiveAmmo      ? 0
                              : barrel == BarrelType::SpreadBarrel ? 6
                                                                   : 25,
                    .traits = ammo == AmmoType::ExplosiveAmmo
                                  ? ProjectileTraits::Explosive
                              : ammo == AmmoType::BouncyAmmo
                                  ? ProjectileTraits::Bouncy
                                  : ProjectileTraits::None,
                },
        };
    }
};
