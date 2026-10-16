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
    void updateBestScore(int score);

    [[nodiscard]] int getBestScore() const noexcept
    {
        return bestScore;
    }

    constexpr bool isWave4StartUnlocked() const noexcept
    {
        return wave4startUnlocked;
    }

    constexpr bool isExtraWeaponUnlocked() const noexcept
    {
        return extraWeaponUnlocked;
    }

    constexpr const std::set<WeaponCombo>& getUnlockedCombos() const noexcept
    {
        return unlockedWeaponCombos;
    }

    UnlockStatus tryUnlockWave4Start()
    {
        return std::exchange(wave4startUnlocked, true)
                   ? UnlockStatus::AlreadyUnlocked
                   : UnlockStatus::Ok;
    }

    UnlockStatus tryUnlockExtraWeapon()
    {
        return std::exchange(extraWeaponUnlocked, true)
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

private:
    int version = 2;
    int bestScore = 0;
    bool wave4startUnlocked = false;
    bool extraWeaponUnlocked = false;
    std::set<WeaponCombo> unlockedWeaponCombos = { WeaponCombo::None };
};
