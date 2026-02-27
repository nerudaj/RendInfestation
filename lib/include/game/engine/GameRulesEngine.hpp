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

    void operator()(const event::PlayerFiredWeapon& e);

    void operator()(const event::ProjectileDestroyed& e);

    void operator()(const event::EnemyAttackLands& e);

    void operator()(const auto&) {}

public:
    void update(const dgm::Time& time);

    void updatePlayer(
        ActorIndexType actorIdx,
        Actor& actor,
        PlayerInventory& inventory,
        const dgm::Time& time);

    void updateNpc(
        ActorIndexType actorIdx,
        Actor& actor,
        NpcInventory& inventory,
        const dgm::Time& time);

    void updateProjectile(
        ActorIndexType actorIdx,
        Actor& actor,
        ProjectileInventory& inventory,
        const dgm::Time& time);

private:
    Weapon& getPlayerWeapon(PlayerInventory& inventory) const
    {
        return inventory.weapons[static_cast<int>(inventory.activeWeapon)];
    }

    void handleProjectileToActorCollision(
        ActorIndexType projectileIdx, ActorIndexType actorIdx);

    void handleDamageMarkerToActorCollision(
        ActorIndexType markerIdx, ActorIndexType actorIdx);

private:
    EventQueue<GameEvent>& eventQueue;
    GameScene& scene;
    const GameTextureAtlas& atlas;
    Input& input;
    SoundPlayer& soundPlayer;
};
