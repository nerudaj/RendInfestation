#pragma once

#include "appstate/Game/enums/WeaponCombo.hpp"
#include <nlohmann/json.hpp>

enum class [[nodiscard]] UnlockStatus : bool
{
    Ok,
    AlreadyUnlocked
};

class [[nodiscard]] SaveManager final
{
public:
    friend void to_json(nlohmann::json& j, const SaveManager& mgr);

    friend void from_json(const nlohmann::json& j, SaveManager& mgr);

public:
    void updateBestScore(size_t mapIdx, int score);

    [[nodiscard]] constexpr int getBestScore(size_t mapIdx) const noexcept
    {
        if (bestScores.size() <= mapIdx) return 0;
        return bestScores[mapIdx];
    }

    [[nodiscard]] constexpr bool isWave4StartUnlocked() const noexcept
    {
        return wave4startUnlocked;
    }

    [[nodiscard]] constexpr bool isExtraWeaponUnlocked() const noexcept
    {
        return extraWeaponUnlocked;
    }

    [[nodiscard]] constexpr bool isReviveUnlocked() const noexcept
    {
        return reviveUnlocked;
    }

    [[nodiscard]] constexpr bool areGoldModulesUnlocked() const noexcept
    {
        return goldModulesUnlocked;
    }

    [[nodiscard]] constexpr bool isHardModeUnlocked() const noexcept
    {
        return hardModeUnlocked;
    }

    constexpr const std::set<WeaponCombo>& getUnlockedCombos() const noexcept
    {
        return unlockedWeaponCombos;
    }

    [[nodiscard]] constexpr bool
    isNthSurvivalMapUnlocked(size_t mapIdx) const noexcept
    {
        return mapIdx < unlockedSurvivalMapCount;
    }

    UnlockStatus tryUnlockWave4Start() noexcept
    {
        return std::exchange(wave4startUnlocked, true)
                   ? UnlockStatus::AlreadyUnlocked
                   : UnlockStatus::Ok;
    }

    UnlockStatus tryUnlockExtraWeapon() noexcept
    {
        return std::exchange(extraWeaponUnlocked, true)
                   ? UnlockStatus::AlreadyUnlocked
                   : UnlockStatus::Ok;
    }

    UnlockStatus tryUnlockRevive() noexcept
    {
        return std::exchange(reviveUnlocked, true)
                   ? UnlockStatus::AlreadyUnlocked
                   : UnlockStatus::Ok;
    }

    UnlockStatus tryUnlockWeaponCombo(const WeaponCombo combo)
    {
        if (unlockedWeaponCombos.contains(combo))
            return UnlockStatus::AlreadyUnlocked;
        unlockedWeaponCombos.insert(combo);
        return UnlockStatus::Ok;
    }

    constexpr UnlockStatus tryUnlockGoldModules() noexcept
    {
        return std::exchange(goldModulesUnlocked, true)
                   ? UnlockStatus::AlreadyUnlocked
                   : UnlockStatus::Ok;
    }

    UnlockStatus tryUnlockHardMode() noexcept
    {
        return std::exchange(hardModeUnlocked, true)
                   ? UnlockStatus::AlreadyUnlocked
                   : UnlockStatus::Ok;
    }

    UnlockStatus tryUnlockSurvivalMap(size_t idx);

private:
    const static inline int version = 6;
    std::vector<int> bestScores = {};
    bool wave4startUnlocked = false;
    bool extraWeaponUnlocked = false;
    bool reviveUnlocked = false;
    bool goldModulesUnlocked = false;
    bool hardModeUnlocked = false;
    std::set<WeaponCombo> unlockedWeaponCombos = { WeaponCombo::None };
    size_t unlockedSurvivalMapCount = 1;
};
