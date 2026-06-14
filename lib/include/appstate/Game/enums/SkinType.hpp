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
    Turret,

    // Projectiles
    SmallBullet,
    BigBullet,
    Landmine,
    Hyperbeam,
    Spikes,
    PinkFireball,
    Fireball,
    Rocket,
    Freeze,

    // Props
    Prop,

    // Effects
    Explosion,

    // Door
    DoorHorizontal,
};
