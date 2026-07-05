#pragma once

#include <nlohmann/json.hpp>

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

private:
    int version = 1;
    int bestScore = 0;
};
