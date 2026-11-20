#include "misc/SaveManager.hpp"
#include "appstate/Game/definitions/Constants.hpp"

namespace Keys
{
    constexpr const char* Version = "version";
    constexpr const char* BestScore = "bestScore";
    constexpr const char* BestScores = "bestScores";
    constexpr const char* Wave4 = "wave4";
    constexpr const char* ExtraWeapon = "extraWeapon";
    constexpr const char* Combos = "combos";
    constexpr const char* Revive = "revive";
    constexpr const char* GoldModules = "goldModules";
    constexpr const char* HardMode = "hardMode";
    constexpr const char* SurvivalMapCount = "unlockedSurvivalMapCount";
}; // namespace Keys

void to_json(nlohmann::json& j, const SaveManager& mgr)
{
    j[Keys::Version] = mgr.version;
    j[Keys::BestScores] = mgr.bestScores;
    j[Keys::Wave4] = mgr.wave4startUnlocked;
    j[Keys::ExtraWeapon] = mgr.extraWeaponUnlocked;
    j[Keys::Combos] = mgr.unlockedWeaponCombos;
    j[Keys::Revive] = mgr.reviveUnlocked;
    j[Keys::GoldModules] = mgr.goldModulesUnlocked;
    j[Keys::HardMode] = mgr.hardModeUnlocked;
    j[Keys::SurvivalMapCount] = mgr.unlockedSurvivalMapCount;

    if (!mgr.goldModulesUnlocked
        && mgr.unlockedWeaponCombos.contains(WeaponCombo::Obliterator))
    {
        throw std::runtime_error(
            "Corrupted save - obliterator cannot be unlocked before gold "
            "modules");
    }
    else if (
        mgr.unlockedSurvivalMapCount < 1
        || mgr.unlockedSurvivalMapCount > MAP_LIMIT)
    {
        throw std::runtime_error(
            "Corrupted save - incorrect number of unlocked maps: "
            + std::to_string(mgr.unlockedSurvivalMapCount));
    }
}

void from_json(const nlohmann::json& j, SaveManager& mgr)
{
    int jsonVersion = 0;
    j.at(Keys::Version).get_to(jsonVersion);

    if (jsonVersion >= 1 && jsonVersion < 5)
    {
        mgr.bestScores.resize(1);
        j.at(Keys::BestScore).get_to(mgr.bestScores[0]);
    }

    if (jsonVersion >= 2)
    {
        j.at(Keys::Wave4).get_to(mgr.wave4startUnlocked);
        j.at(Keys::ExtraWeapon).get_to(mgr.extraWeaponUnlocked);
        j.at(Keys::Combos).get_to(mgr.unlockedWeaponCombos);
    }

    if (jsonVersion >= 3)
    {
        j.at(Keys::Revive).get_to(mgr.reviveUnlocked);
    }

    if (jsonVersion >= 4)
    {
        j.at(Keys::SurvivalMapCount).get_to(mgr.unlockedSurvivalMapCount);
    }

    if (jsonVersion >= 5)
    {
        j.at(Keys::BestScores).get_to(mgr.bestScores);
    }

    if (jsonVersion >= 6)
    {
        j.at(Keys::GoldModules).get_to(mgr.goldModulesUnlocked);
        j.at(Keys::HardMode).get_to(mgr.hardModeUnlocked);
    }
}

void SaveManager::updateBestScore(size_t mapIdx, int score)
{
    assert(mapIdx < MAP_LIMIT);
    if (bestScores.size() <= mapIdx)
    {
        bestScores.resize(mapIdx + 1);
    }

    assert(bestScores[mapIdx] < score);
    bestScores[mapIdx] = score;
}

UnlockStatus SaveManager::tryUnlockSurvivalMap(size_t idx)
{
    // If this fails then somehow we are playing map that shouldn't be
    // unlocked yet
    assert(idx <= unlockedSurvivalMapCount);
    assert(idx != MAP_LIMIT);

    if (idx < unlockedSurvivalMapCount) return UnlockStatus::AlreadyUnlocked;

    ++unlockedSurvivalMapCount;
    return UnlockStatus::Ok;
}
