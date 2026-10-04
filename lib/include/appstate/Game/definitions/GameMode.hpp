#pragma once

#include <string>

enum class [[nodiscard]] GameMode
{
    Story,
    Survival,
};

struct [[nodiscard]] GameBoons final
{
    bool wave4start = false;
    bool wave8start = false;
    bool extraWeapon = false;
};

struct [[nodiscard]] GameModeProperties final
{
    GameMode mode;
    std::string mapName;
    GameBoons boons;
};
