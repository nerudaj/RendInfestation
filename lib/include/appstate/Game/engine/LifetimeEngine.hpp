#pragma once

#include "appstate/Game/definitions/GameScene.hpp"
#include "appstate/Game/definitions/GameTextureAtlas.hpp"
#include "misc/EventQueue.hpp"
#include <DGM/classes/Time.hpp>

// System responsible for updating entities with Lifetime component
class [[nodiscard]] LifetimeEngine final
{
public:
    LifetimeEngine(
        GameScene& scene,
        EventQueue<GameEvent>& eventQueue,
        const GameTextureAtlas& atlas) noexcept
        : scene(scene), eventQueue(eventQueue), atlas(atlas)
    {
    }

public:
    void update(const dgm::Time& time);

private:
    GameScene& scene;
    EventQueue<GameEvent>& eventQueue;
    const GameTextureAtlas& atlas;
};
