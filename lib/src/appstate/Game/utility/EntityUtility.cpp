#include "appstate/Game/utility/EntityUtility.hpp"
#include "appstate/Game/builders/ActorBuilder.hpp"
#include "appstate/Game/definitions/Constants.hpp"

void EntityUtility::destroyProp(entt::registry& actors, entt::entity entity)
{
    auto& skin = actors.get<Skin>(entity);
    if (skin.animation.getStateName() == "cactus-pot")
    {
        actors.remove<Health>(entity);

        auto& collider = actors.get<Collider>(entity);
        collider.options.semighost = SEMIGHOST_PROJECTILE;
        skin.animation.setState("cactus-pot-destroyed", "looping"_true);

        ActorBuilder::createParticleSystem(
            actors,
            collider.getPosition(),
            sf::Vector2f { 1.f, 0.f },
            ParticleSystemKind::CactusSpatter);
    }
    else if (skin.animation.getStateName() == "tree")
    {
        actors.remove<Health>(entity);

        auto& collider = actors.get<Collider>(entity);
        collider.options.nonblocking = true;
        collider.options.disabled = true;
        skin.animation.setState("tree-destroyed", "looping"_true);

        ActorBuilder::createParticleSystem(
            actors,
            collider.getPosition(),
            sf::Vector2f { 1.f, 0.f },
            ParticleSystemKind::CactusSpatter);
    }
}

void EntityUtility::shatterProjectile(
    entt::registry& actors, entt::entity projectile)
{
    auto body = actors.get<PhysicsBody>(projectile);
    auto skin = actors.get<Skin>(projectile);
    skin.scale *= 0.5f;
    auto inventory = actors.get<ProjectileInventory>(projectile);

    inventory.damage /= 2;
    inventory.traits = inventory.traits - ProjectileTraits::SplitOnHit;

    auto collider = actors.get<Collider>(projectile);
    collider.options.disabled = true; // gets reenabled via TimedScript
    auto&& newEntities = std::vector<entt::entity>();
    body.forward = body.forward.rotatedBy(sf::degrees(45));

    for (auto&& _ : std::views::iota(0u, 4u))
    {
        auto entity = actors.create();
        actors.emplace<Collider>(entity, collider);

        body.forward = body.forward.rotatedBy(sf::degrees(90));
        actors.emplace<PhysicsBody>(entity, body);

        actors.emplace<Lifetime>(entity, actors.get<Lifetime>(projectile));
        actors.emplace<ZIndex>(entity, actors.get<ZIndex>(projectile));
        actors.emplace<Skin>(entity, skin);
        actors.emplace<ProjectileInventory>(entity, inventory);

        if (auto interval = actors.try_get<Interval>(projectile))
        {
            actors.emplace<Interval>(entity, *interval);
        }

        newEntities.push_back(entity);
    }

    // Re-enable colliders after some time
    auto future = actors.create();
    actors.emplace<TimedScript>(
        future,
        sf::seconds(0.1f),
        [entities = newEntities, &actors]
        {
            for (auto&& entity : entities)
            {
                auto&& collider = actors.try_get<Collider>(entity);
                if (!collider) continue;

                collider->options.disabled = false;
            }
        });
}

void EntityUtility::shatterBounceProjectile(
    entt::registry& actors, entt::entity projectile)
{
    auto body = actors.get<PhysicsBody>(projectile);
    auto skin = actors.get<Skin>(projectile);
    skin.scale *= 0.5f;
    auto inventory = actors.get<ProjectileInventory>(projectile);

    inventory.damage /= 2;
    inventory.traits = inventory.traits - ProjectileTraits::ShrapnellingBounce;

    auto collider = actors.get<Collider>(projectile);
    collider.options.disabled = true; // gets reenabled via TimedScript
    auto&& newEntities = std::vector<entt::entity>();
    body.forward = body.forward.rotatedBy(sf::degrees(45));

    for (auto&& _ : std::views::iota(0u, 4u))
    {
        auto entity = actors.create();
        actors.emplace<Collider>(entity, collider);

        body.forward = body.forward.rotatedBy(sf::degrees(90));
        actors.emplace<PhysicsBody>(entity, body);

        actors.emplace<Lifetime>(entity, actors.get<Lifetime>(projectile));
        actors.emplace<ZIndex>(entity, actors.get<ZIndex>(projectile));
        actors.emplace<Skin>(entity, skin);
        actors.emplace<ProjectileInventory>(entity, inventory);

        if (auto interval = actors.try_get<Interval>(projectile))
        {
            actors.emplace<Interval>(entity, *interval);
        }

        newEntities.push_back(entity);
    }

    // Re-enable colliders after some time
    auto future = actors.create();
    actors.emplace<TimedScript>(
        future,
        sf::seconds(0.1f),
        [entities = newEntities, &actors]
        {
            for (auto&& entity : entities)
            {
                auto&& collider = actors.try_get<Collider>(entity);
                if (!collider) continue;

                collider->options.disabled = false;
            }
        });
}

void EntityUtility::spawnDamageMarkerForProjectile(
    entt::registry& actors,
    entt::entity projectile,
    ProjectileInventory* inventory)
{
    // For passthru projectiles, there is a brief period of ignoring
    // hits after the last one
    auto interval = actors.try_get<Interval>(projectile);
    if (interval)
    {
        if (interval->timer > sf::Time::Zero) return;
        interval->timer = interval->delay;
    }

    const auto& skin = actors.get<Skin>(projectile);
    const auto& collider = actors.get<Collider>(projectile);
    const float scaleFactor =
        (inventory->traits & ProjectileTraits::Big ? 2.f : 1.f) * skin.scale;

    const float markerRadius =
        inventory->traits & ProjectileTraits::Explosive ? BASE_EXPLOSION_RADIUS
        : skin.skinType == SkinType::Fireball ? BASE_FIREBALL_BLAST_RADIUS
                                              : collider.getRadius();

    ActorBuilder::createDamageMarker(
        actors,
        dgm::Math::toUnit(actors.get<PhysicsBody>(projectile).forward),
        collider.getPosition(),
        markerRadius * scaleFactor,
        *inventory);
}
