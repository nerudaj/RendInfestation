#pragma once

enum class [[nodiscard]] ActorKind : unsigned char
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
