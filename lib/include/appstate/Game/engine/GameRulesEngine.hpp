#pragma once

#include "appstate/Game/Types.hpp"
#include "appstate/Game/definitions/GameEvents.hpp"
#include "appstate/Game/definitions/GameScene.hpp"
#include "appstate/Game/definitions/GameTextureAtlas.hpp"
#include "appstate/Game/enums/WeaponModule.hpp"
#include "appstate/Game/survival/SurvivalGameDirector.hpp"
#include "appstate/Game/utility/AudioUtility.hpp"
#include "audio/SoundPlayer.hpp"
#include "input/Input.hpp"
#include "misc/EventQueue.hpp"
#include "misc/SaveManager.hpp"
#include "strings/StringProvider.hpp"

/*
 * NOTE for future me
 *
 * All these engines should've been called "systems"
 * because ecs stands for entity-component-system.
 *
 * I do have entities and components relatively
 * well put together, but engines should've been systems and they should've been
 * smaller.
 *
 * Maybe they could only have templated update methods
 * to denote what is actually used? But I dunno how to handle events
 * intelligently.
 *
 * Also - put all entites under "entites" folder and namespace instead of
 * definitions.
 *
 * An extra thought - can I intermix ECS with event queue? If every single
 * system participates in event handling then it could arbitrarily prolong
 * runtime without added benefit. Shall event handlers be always logically
 * separated from systems, even if they are implemented by the same classes (and
 * should they be implemented by the same classes if those classes don't have
 * internal state?)
 */
class [[nodiscard]] GameRulesEngine final
{
public:
    GameRulesEngine(
        EventQueue<GameEvent>& eventQueue,
        GameScene& scene,
        const GameTextureAtlas& atlas,
        Input& input,
        AudioUtility& audio,
        const StringProvider<StringId>& strings,
        SaveManager& saveManager,
        const size_t mapIdx) noexcept
        : eventQueue(eventQueue)
        , scene(scene)
        , atlas(atlas)
        , input(input)
        , audio(audio)
        , strings(strings)
        , saves(saveManager)
        , mapIdx(mapIdx)
    {
    }

    GameRulesEngine(GameRulesEngine&&) = default;
    GameRulesEngine(const GameRulesEngine&) = delete;

public:
    void operator()(const event::ActorToMeshCollision& e);

    void operator()(const event::ActorToActorCollision& e);

    void operator()(const event::ActorBounced& e);

    void operator()(const event::ActorStartedAttack& e);

    void operator()(const event::ActorFiredWeapon& e);

    void operator()(const event::ProjectileDestroyed& e);

    void operator()(const event::DoorOpened& e);

    void operator()(const event::DoorStartsClosing& e);

    void operator()(const event::ActorIsFalling& e);

    void operator()(const event::ObjectDestroyed& e);

    void operator()(const event::SurvivalSpawnerTimerHit& e);

    void operator()(const event::ActorDamaged& e);

    void operator()(const event::WaveEnded& e);

    void operator()(const event::PlayerKilled& e);

    void operator()(const event::PlayerKilledForGood& e);

    void operator()(const auto&) {}

public:
    void update(const dgm::Time& time);

    void updateEntitiesWithInput(const dgm::Time& time);

    void updateTriggers(const dgm::Time& time);

    void postMessage(StringId messageId, bool updateLedger = true);

private:
    void updateMessages(const dgm::Time& time);

    Weapon& getActiveWeapon(WeaponInventory& inventory) const
    {
        return inventory.weapons[inventory.activeWeapon];
    }

    void handleProjectileToActorCollision(
        entt::entity projectileIdx, entt::entity actorIdx);

    void handleDamageMarkerToActorCollision(
        entt::entity markerIdx, entt::entity actorIdx);

    void handleTriggerToActorCollision(
        entt::entity triggerIdx, entt::entity actorIdx);

    sf::Vector2f pickEnemySpawnPosition() const;

    bool shallGainEmergencyRevive() const noexcept;

    [[nodiscard]] sf::Vector2f getIdealPlayerRespawnLocation() const;

private:
    EventQueue<GameEvent>& eventQueue;
    GameScene& scene;
    const GameTextureAtlas& atlas;
    Input& input;
    AudioUtility& audio;
    const StringProvider<StringId>& strings;
    SaveManager& saves;
    const size_t mapIdx;
};
