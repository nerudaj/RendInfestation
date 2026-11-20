#pragma once

#include "appstate/Game/enums/WeaponModule.hpp"
#include <array>

constexpr const auto COMBO_SWEEPER = std::array {
    WeaponModule::Push,
    WeaponModule::BigBullet,
    WeaponModule::PassthruAmmo,
};

constexpr const auto COMBO_LANDMINE = std::array {
    WeaponModule::Spikes,
    WeaponModule::ExplosiveAmmo,
    WeaponModule::SpreadBarrel_x2,
};

constexpr const auto COMBO_FRAGSHOT = std::array {
    WeaponModule::Spikes,
    WeaponModule::Splitter,
    WeaponModule::SpreadBarrel_x4,
};

constexpr const auto COMBO_PAINLESS = std::array {
    WeaponModule::CadenceBarrel,
    WeaponModule::Ricochet,
    WeaponModule::SpreadBarrel_x2,
};

constexpr const auto COMBO_WORST_EVER = std::array {
    WeaponModule::Spikes,
    WeaponModule::ExplosiveAmmo,
    WeaponModule::PassthruAmmo,
};

constexpr const auto COMBO_TOWER = std::array {
    WeaponModule::CadenceBarrel,
    WeaponModule::Turret,
    WeaponModule::Push,
};

constexpr const auto COMBO_OBLITERATOR = std::array {
    WeaponModule::AutomaticShotgun,
    WeaponModule::ShatteringRicochet,
    WeaponModule::PulseRounds,
};
