#pragma once

enum class [[nodiscard]] SkinType : unsigned char
{
    // Player
    PlayerDefault,
    PlayerLower,

    // Npcs
    Bighead,
    Scuttlebug,
    ScuttlebugBlue,
    Beholder,
    Turret,

    // Projectiles
    SmallBullet,
    Landmine,
    Hyperbeam,
    Spikes,
    Fireball,
    Rocket,
    Freeze,
    TurretSpawner,

    // Props
    Prop,

    // Door
    DoorHorizontal,
};
