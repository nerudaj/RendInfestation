#pragma once

enum class [[nodiscard]] EntityKind : unsigned char
{
    None,
    Prop,
    Player,
    Npc,
    Projectile,
    Effect,
    DamageMarker,
    Door,
};
