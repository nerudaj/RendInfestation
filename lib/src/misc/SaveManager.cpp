#include "misc/SaveManager.hpp"
#include "appstate/Game/definitions/Constants.hpp"

void to_json(nlohmann::json& j, const SaveManager& mgr)
{
    j["version"] = mgr.version;
    j["bestScores"] = mgr.bestScores;
    j["wave4"] = mgr.wave4startUnlocked;
    j["extraWeapon"] = mgr.extraWeaponUnlocked;
    j["combos"] = mgr.unlockedWeaponCombos;
    j["revive"] = mgr.reviveUnlocked;
    j["goldModules"] = mgr.goldModulesUnlocked;
    j["hardMode"] = mgr.hardModeUnlocked;
    j["unlockedSurvivalMapCount"] = mgr.unlockedSurvivalMapCount;

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
    j.at("version").get_to(jsonVersion);

    if (jsonVersion >= 1 && jsonVersion < 5)
    {
        mgr.bestScores.resize(1);
        j.at("bestScore").get_to(mgr.bestScores[0]);
    }

    if (jsonVersion >= 2)
    {
        j.at("wave4").get_to(mgr.wave4startUnlocked);
        j.at("extraWeapon").get_to(mgr.extraWeaponUnlocked);
        j.at("combos").get_to(mgr.unlockedWeaponCombos);
    }

    if (jsonVersion >= 3)
    {
        j.at("revive").get_to(mgr.reviveUnlocked);
    }

    if (jsonVersion >= 4)
    {
        j.at("unlockedSurvivalMapCount").get_to(mgr.unlockedSurvivalMapCount);
    }

    if (jsonVersion >= 5)
    {
        j.at("bestScores").get_to(mgr.bestScores);
    }

    if (jsonVersion >= 6)
    {
        j.at("goldModules").get_to(mgr.goldModulesUnlocked);
        j.at("hardMode").get_to(mgr.hardModeUnlocked);
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
