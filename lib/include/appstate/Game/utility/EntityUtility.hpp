#pragma once

#include "appstate/Game/definitions/Components.hpp"
#include <SFML/System/Time.hpp>
#include <entt/entt.hpp>

class [[nodiscard]] EntityUtility final
{
public:
    static void destroyProp(entt::registry& actors, entt::entity entity);

    static void
    shatterProjectile(entt::registry& actors, entt::entity projectile);

    static void
    shatterBounceProjectile(entt::registry& actors, entt::entity projectile);

    template<class EffectType>
    static void applyStatusEffect(
        entt::registry& actors, entt::entity target, const sf::Time duration)
    {
        actors.emplace<EffectType>(target);

        auto future = actors.create();
        actors.emplace<TimedScript>(
            future,
            duration,
            [target = target, &actors] { actors.erase<EffectType>(target); });
    }

    static void spawnDamageMarkerForProjectile(
        entt::registry& actors,
        entt::entity projectile,
        ProjectileInventory* inventory);
};
