#pragma once

#include "audio/SoundPlayer.hpp"
#include "game/Types.hpp"
#include "game/definitions/GameEvents.hpp"
#include "game/definitions/GameScene.hpp"
#include "game/definitions/GameTextureAtlas.hpp"
#include "input/Input.hpp"
#include "misc/EventQueue.hpp"

class [[nodiscard]] GameRulesEngine final
{
public:
    GameRulesEngine(
        EventQueue<GameEvent>& eventQueue,
        GameScene& scene,
        const GameTextureAtlas& atlas,
        Input& input,
        SoundPlayer& player) noexcept
        : eventQueue(eventQueue)
        , scene(scene)
        , atlas(atlas)
        , input(input)
        , soundPlayer(player)
    {
    }

    GameRulesEngine(GameRulesEngine&&) = delete;
    GameRulesEngine(const GameRulesEngine&) = delete;

public:
    void operator()(const event::ActorToMeshCollision& e);

    void operator()(const event::ActorToActorCollision& e);

    void operator()(const event::ActorStartedAttack& e);

    void operator()(const event::ActorFiredWeapon& e);

    void operator()(const event::ProjectileDestroyed& e);

    void operator()(const event::DoorOpened& e);

    void operator()(const event::DoorStartsClosing& e);

    void operator()(const auto&) {}

public:
    void update(const dgm::Time& time);

private:
    void updateSpawner(const dgm::Time& time);

    void updateLifetimes(const dgm::Time& time);

    Weapon& getActiveWeapon(WeaponInventory& inventory) const
    {
        return inventory.weapons[inventory.activeWeapon];
    }

    void handleProjectileToActorCollision(
        entt::entity projectileIdx, entt::entity actorIdx);

    void createDamageMarkerForProjectile(
        entt::entity projectile, ProjectileInventory* inventory);

    void handleDamageMarkerToActorCollision(
        entt::entity markerIdx, entt::entity actorIdx);

    void handleTriggerToActorCollision(
        entt::entity triggerIdx, entt::entity actorIdx);

private:
    EventQueue<GameEvent>& eventQueue;
    GameScene& scene;
    const GameTextureAtlas& atlas;
    Input& input;
    SoundPlayer& soundPlayer;
};
