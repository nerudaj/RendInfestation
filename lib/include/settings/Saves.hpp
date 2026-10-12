#pragma once

struct [[nodiscard]] BoonUnlocks final
{
    bool wave4start = true;
    bool wave8start = true;
    bool extraWeapon = true;
    bool sweeperWeapon = true;
};

struct [[nodiscard]] Saves final
{
    BoonUnlocks unlockedBoons;
};
