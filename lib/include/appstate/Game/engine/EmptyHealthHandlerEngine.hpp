#pragma once

#include "appstate/Game/definitions/GameScene.hpp"
#include "appstate/Game/definitions/GameTextureAtlas.hpp"
#include "appstate/Game/utility/AudioUtility.hpp"
#include "misc/EventQueue.hpp"
#include <DGM/classes/Time.hpp>

// System responsible for updating entities with Lifetime component
class [[nodiscard]] EmptyHealthHandlerEngine final
{
public:
    EmptyHealthHandlerEngine(
        GameScene& scene,
        EventQueue<GameEvent>& eventQueue,
        AudioUtility& audio) noexcept
        : scene(scene), eventQueue(eventQueue), audio(audio)
    {
    }

public:
    void update(const dgm::Time& time);

private:
    GameScene& scene;
    EventQueue<GameEvent>& eventQueue;
    AudioUtility& audio;
};
