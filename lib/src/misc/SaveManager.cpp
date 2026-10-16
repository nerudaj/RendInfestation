#include "misc/SaveManager.hpp"

void to_json(nlohmann::json& j, const SaveManager& mgr)
{
    j["version"] = mgr.version;
    j["bestScore"] = mgr.bestScore;
    j["wave4"] = mgr.wave4startUnlocked;
    j["extraWeapon"] = mgr.extraWeaponUnlocked;
    j["combos"] = mgr.unlockedWeaponCombos;
}

void from_json(const nlohmann::json& j, SaveManager& mgr)
{
    j.at("version").get_to(mgr.version);

    if (mgr.version >= 1)
    {
        j.at("bestScore").get_to(mgr.bestScore);
    }
    else if (mgr.version >= 2)
    {
        j.at("wave4").get_to(mgr.wave4startUnlocked);
        j.at("extraWeapon").get_to(mgr.extraWeaponUnlocked);
        j.at("combos").get_to(mgr.unlockedWeaponCombos);
    }
}

void SaveManager::updateBestScore(int score)
{
    assert(bestScore < score);
    bestScore = score;
}
