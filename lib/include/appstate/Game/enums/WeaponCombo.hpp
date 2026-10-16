#pragma once

#include "nlohmann/json.hpp"
#include "strings/StringId.hpp"

enum class [[nodiscard]] WeaponCombo
{
    None,
    Random,
    Sweeper,   // Push + Big + Passthru
    Landmine,  // Spikes + Explosive + Shotgun x2
    Fragshot,  // Spikes + Splitter + Shotgun x4
    Painless,  // Cadence + Shotgun x2 + Bounce
    WorstEver, // Spikes + Explosive + Passthru
    Tower,     // Turret + Cadence + Push
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    WeaponCombo,
    {
        { WeaponCombo::None, "none" },
        { WeaponCombo::Random, "random" },
        { WeaponCombo::Sweeper, "sweeper" },
        { WeaponCombo::Landmine, "landmine" },
        { WeaponCombo::Fragshot, "fragshot" },
        { WeaponCombo::Painless, "painless" },
        { WeaponCombo::WorstEver, "worst" },
        { WeaponCombo::Tower, "tower" },
    });

static inline StringId getWeaponComboName(const WeaponCombo combo)
{
    switch (combo)
    {
        using enum WeaponCombo;

    case None:
        return StringId::StartingWeapon_None;
    case Random:
        return StringId::StartingWeapon_Random;
    case Sweeper:
        return StringId::StartingWeapon_Sweeper;
    case Landmine:
        return StringId::StartingWeapon_Landmine;
    case Fragshot:
        return StringId::StartingWeapon_Fragshot;
    case Painless:
        return StringId::StartingWeapon_Painless;
    case WorstEver:
        return StringId::StartingWeapon_WorstEver;
    case Tower:
        return StringId::StartingWeapon_Tower;
    default:
        assert(false);
    }
}
