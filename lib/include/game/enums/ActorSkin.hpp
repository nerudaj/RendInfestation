#pragma once

enum class [[nodiscard]] ActorSkin : unsigned char
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
};
