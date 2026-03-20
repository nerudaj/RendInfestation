#pragma once

#include <string>

enum class GameMode
{
    Story,
    Survival,
};

struct GameModeProperties
{
    GameMode mode;
    std::string mapName;
};
