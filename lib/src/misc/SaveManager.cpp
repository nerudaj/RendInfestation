#include "misc/SaveManager.hpp"

void to_json(nlohmann::json& j, const SaveManager& mgr)
{
    j["version"] = mgr.version;
    j["bestScore"] = mgr.bestScore;
}

void from_json(const nlohmann::json& j, SaveManager& mgr)
{
    j.at("version").get_to(mgr.version);

    if (mgr.version >= 1)
    {
        j.at("bestScore").get_to(mgr.bestScore);
    }
}

void SaveManager::updateBestScore(int score)
{
    assert(bestScore < score);
    bestScore = score;
}
