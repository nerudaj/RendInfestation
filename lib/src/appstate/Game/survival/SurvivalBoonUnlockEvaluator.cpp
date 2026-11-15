#include "appstate/Game/survival/SurvivalBoonUnlockEvaluator.hpp"
#include <array>

SurvivalBoonUnlockEvaluator::SurvivalBoonUnlockEvaluator(
    SaveManager& saves) noexcept
    : saves(saves)
{
}

void SurvivalBoonUnlockEvaluator::evaluateUnlocks(
    const int beatenWaveNum,
    const size_t mapIdx,
    const PlayerInventory& playerInventory,
    std::function<void(StringId)>&& postMessage)
{
    if (playerInventory.loadout.weapons.size() == 2
        && saves.tryUnlockExtraWeapon() == UnlockStatus::Ok)
    {
        postMessage(StringId::ExtraWeaponUnlocked);
    }

    if (beatenWaveNum == 4 && saves.tryUnlockWave4Start() == UnlockStatus::Ok)
    {
        postMessage(StringId::Wave4SkipUnlocked);
    }

    if (beatenWaveNum == 12 && mapIdx < (MAP_LIMIT - 1)
        && saves.tryUnlockSurvivalMap(mapIdx + 1) == UnlockStatus::Ok)
    {
        postMessage(StringId::NewMapUnlocked);
    }

    if (beatenWaveNum == 12 && mapIdx == 3
        && saves.tryUnlockHardMode() == UnlockStatus::Ok)
    {
        postMessage(StringId::HardModeUnlocked);
    }

    const bool allLoadoutSlotsUsed = std::ranges::any_of(
        playerInventory.loadout.weapons,
        [](const WeaponConfig& weapon)
        {
            // true if all slots are used
            return std::find(
                       weapon.modules.begin(),
                       weapon.modules.end(),
                       WeaponModule::None)
                   == weapon.modules.end();
        });

    if (allLoadoutSlotsUsed
        && saves.tryUnlockWeaponCombo(WeaponCombo::Random) == UnlockStatus::Ok)
    {
        postMessage(StringId::NewWeaponComboUnlocked);
    }

    auto&& hasCombo =
        [this, &playerInventory](const std::array<WeaponModule, 3u>& combo)
    {
        const auto comboSet = combo | std::ranges::to<std::set>();

        return std::any_of(
            playerInventory.loadout.weapons.begin(),
            playerInventory.loadout.weapons.end(),
            [&comboSet](const WeaponConfig& config)
            {
                return std::all_of(
                    config.modules.begin(),
                    config.modules.end(),
                    [&comboSet](const WeaponModule& module)
                    { return comboSet.contains(module); });
            });
    };

    using enum WeaponModule;
    if (hasCombo({ Push, BigBullet, PassthruAmmo })
        && saves.tryUnlockWeaponCombo(WeaponCombo::Sweeper) == UnlockStatus::Ok)
    {
        postMessage(StringId::NewWeaponComboUnlocked);
    }

    if (hasCombo({ Spikes, ExplosiveAmmo, SpreadBarrel_x2 })
        && saves.tryUnlockWeaponCombo(WeaponCombo::Landmine)
               == UnlockStatus::Ok)
    {
        postMessage(StringId::NewWeaponComboUnlocked);
    }

    if (hasCombo({ Spikes, Splitter, SpreadBarrel_x4 })
        && saves.tryUnlockWeaponCombo(WeaponCombo::Fragshot)
               == UnlockStatus::Ok)
    {
        postMessage(StringId::NewWeaponComboUnlocked);
    }

    if (hasCombo({ CadenceBarrel, Ricochet, SpreadBarrel_x2 })
        && saves.tryUnlockWeaponCombo(WeaponCombo::Painless)
               == UnlockStatus::Ok)
    {
        postMessage(StringId::NewWeaponComboUnlocked);
    }

    if (hasCombo({ Spikes, ExplosiveAmmo, PassthruAmmo })
        && saves.tryUnlockWeaponCombo(WeaponCombo::WorstEver)
               == UnlockStatus::Ok)
    {
        postMessage(StringId::NewWeaponComboUnlocked);
    }

    if (hasCombo({ Turret, CadenceBarrel, Push })
        && saves.tryUnlockWeaponCombo(WeaponCombo::Tower) == UnlockStatus::Ok)
    {
        postMessage(StringId::NewWeaponComboUnlocked);
    }

    if (hasCombo({ AutomaticShotgun, ShatteringRicochet, PulseRounds })
        && saves.tryUnlockWeaponCombo(WeaponCombo::Obliterator)
               == UnlockStatus::Ok)
    {
        postMessage(StringId::NewWeaponComboUnlocked);
    }

    // Note: this unlocks when the player unlocks all base combos
    // obliterator is from gold modules, so impossible to get at that stage
    // but it makes for a great delimiter
    if (saves.getUnlockedCombos().size()
            == std::to_underlying(WeaponCombo::Obliterator)
        && saves.tryUnlockGoldModules() == UnlockStatus::Ok)
    {
        postMessage(StringId::GoldModulesUnlocked);
    }

    if (playerInventory.emergencyReviveActive
        && saves.tryUnlockRevive() == UnlockStatus::Ok)
    {
        postMessage(StringId::StartingEmergencyReviveUnlocked);
    }
}
