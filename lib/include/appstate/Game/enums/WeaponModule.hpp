#pragma once

#include "strings/StringId.hpp"
#include <array>
#include <misc/Compatibility.hpp>
#include <stdexcept>

enum class [[nodiscard]] WeaponModule
{
    None = -1,
    SpreadBarrel_x4,
    SpreadBarrel_x2,
    CadenceBarrel,
    Ricochet,
    PassthruAmmo,
    BigBullet,
    Splitter,
    Spikes,
    Push,
    Dummy,
    ExplosiveAmmo,
    Turret,
    ExtraHealth,
    ExtraGun,
};

constexpr auto ALLOWED_MODULES = std::array {
    WeaponModule::None,
    WeaponModule::SpreadBarrel_x2,
    WeaponModule::SpreadBarrel_x4,
    WeaponModule::CadenceBarrel,
    WeaponModule::ExplosiveAmmo,
    WeaponModule::Ricochet,
    WeaponModule::PassthruAmmo,
    WeaponModule::BigBullet,
    WeaponModule::Spikes,
    WeaponModule::Splitter,
    WeaponModule::Push,
    WeaponModule::Turret,
};

static inline StringId getModuleName(WeaponModule module)
{
    switch (module)
    {
    case WeaponModule::None:
        return StringId::WeaponModule_None;
    case WeaponModule::SpreadBarrel_x4:
        return StringId::WeaponModule_SpreadBarrel;
    case WeaponModule::SpreadBarrel_x2:
        return StringId::WeaponModule_SpreadBarrelx2;
    case WeaponModule::CadenceBarrel:
        return StringId::WeaponModule_CadenceBarrel;
    case WeaponModule::Ricochet:
        return StringId::WeaponModule_Ricochet;
    case WeaponModule::PassthruAmmo:
        return StringId::WeaponModule_PassthruAmmo;
    case WeaponModule::BigBullet:
        return StringId::WeaponModule_BigBullet;
    case WeaponModule::Splitter:
        return StringId::WeaponModule_Splitter;
    case WeaponModule::Spikes:
        return StringId::WeaponModule_Spikes;
    case WeaponModule::Push:
        return StringId::WeaponModule_Push;
    case WeaponModule::ExplosiveAmmo:
        return StringId::WeaponModule_ExplosiveAmmo;
    case WeaponModule::Turret:
        return StringId::WeaponModule_Turret;
    case WeaponModule::ExtraHealth:
        return StringId::WeaponModule_ExtraHealth;
    case WeaponModule::ExtraGun:
        return StringId::WeaponModule_ExtraGun;
    }

    throw std::runtime_error(uni::format(
        "getModuleNames: Module {} not handled", std::to_underlying(module)));
}

static inline StringId getModuleDescription(WeaponModule module)
{
    switch (module)
    {
    case WeaponModule::None:
        return StringId::WeaponModuleDesc_None;
    case WeaponModule::SpreadBarrel_x4:
        return StringId::WeaponModuleDesc_SpreadBarrelx4;
    case WeaponModule::SpreadBarrel_x2:
        return StringId::WeaponModuleDesc_SpreadBarrelx2;
    case WeaponModule::CadenceBarrel:
        return StringId::WeaponModuleDesc_CadenceBarrel;
    case WeaponModule::Ricochet:
        return StringId::WeaponModuleDesc_Ricochet;
    case WeaponModule::PassthruAmmo:
        return StringId::WeaponModuleDesc_PassthruAmmo;
    case WeaponModule::BigBullet:
        return StringId::WeaponModuleDesc_BigBullet;
    case WeaponModule::Splitter:
        return StringId::WeaponModuleDesc_Splitter;
    case WeaponModule::Spikes:
        return StringId::WeaponModuleDesc_Spikes;
    case WeaponModule::Push:
        return StringId::WeaponModuleDesc_Push;
    case WeaponModule::ExplosiveAmmo:
        return StringId::WeaponModuleDesc_ExplosiveAmmo;
    case WeaponModule::Turret:
        return StringId::WeaponModuleDesc_Turret;
    case WeaponModule::ExtraHealth:
        return StringId::WeaponModuleDesc_ExtraHealth;
    case WeaponModule::ExtraGun:
        return StringId::WeaponModuleDesc_ExtraGun;
    }

    throw std::runtime_error(uni::format(
        "getModuleNames: Module {} not handled", std::to_underlying(module)));
}
