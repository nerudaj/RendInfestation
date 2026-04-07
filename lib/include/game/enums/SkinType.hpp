#pragma once

enum class [[nodiscard]] SkinType : unsigned char
{
    // Player
    PlayerDefault,

    // Npcs
    Bighead,
    Scuttlebug,
    ScuttlebugBlue,
    Beholder,

    // Projectiles
    SmallBullet,
    BigBullet,
    Landmine,
    Hyperbeam,
    Spikes,

    // Props
    Prop,

    // Effects
    Explosion,

    // Door
    DoorHorizontal,
};
