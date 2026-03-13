#pragma once

enum class [[nodiscard]] SkinType : unsigned char
{
    // Player
    PlayerDefault,

    // Npcs
    Bighead,

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
