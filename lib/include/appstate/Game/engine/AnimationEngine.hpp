#pragma once

#include "appstate/Game/definitions/GameEvents.hpp"
#include "appstate/Game/definitions/GameScene.hpp"
#include "appstate/Game/definitions/GameTextureAtlas.hpp"
#include "misc/EventQueue.hpp"

class [[nodiscard]] AnimationEngine final
{
public:
    AnimationEngine(
        GameScene& scene,
        EventQueue<GameEvent>& eventQueue,
        const GameTextureAtlas& atlas)
        : scene(scene), eventQueue(eventQueue), atlas(atlas)
    {
    }

public:
    void operator()(const event::ActorFiredWeapon& e);

    void operator()(const event::ProjectileDestroyed& e);

    void operator()(const event::ActorStartedAttack& e);

    void operator()(const event::ActorFinishedAttack& e);

    void operator()(const event::ActorMoved& e);

    void operator()(const event::ActorStopped& e);

    void operator()(const event::ActorDamaged& e);

    void operator()(const auto&) {}

public:
    void update(const dgm::Time& time);

private:
    GameScene& scene;
    EventQueue<GameEvent>& eventQueue;
    const GameTextureAtlas& atlas;
};
