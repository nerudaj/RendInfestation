#pragma once

#include "audio/SoundPlayer.hpp"
#include "appstate/Game/Types.hpp"
#include "appstate/Game/definitions/GameEvents.hpp"
#include "appstate/Game/definitions/GameScene.hpp"
#include "appstate/Game/definitions/GameTextureAtlas.hpp"
#include "appstate/Game/enums/WeaponModule.hpp"
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

    void operator()(const event::ActorIsFalling& e);

    void operator()(const event::ObjectDestroyed& e);

    void operator()(const auto&) {}

public:
    void update(const dgm::Time& time);

    void updateEntitiesWithInput(const dgm::Time& time);

    void updateTriggers(const dgm::Time& time);

    void updateHealth();

private:
    void updateSpawner(const dgm::Time& time);

    void updateLifetimes(const dgm::Time& time);

    void tryUnlockRandomModule();

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

    void postMessage(const std::string& message)
    {
        scene.hudMessage.text = message;
        scene.hudMessage.displayTime = sf::seconds(3.f);
    }

    int getScoreForEnemy(const Skin& skin) const;

    sf::Vector2f pickEnemySpawnPosition() const;

private:
    EventQueue<GameEvent>& eventQueue;
    GameScene& scene;
    const GameTextureAtlas& atlas;
    Input& input;
    SoundPlayer& soundPlayer;
};
