#pragma once

#include "game/definitions/GameEvent.hpp"
#include "game/definitions/GameScene.hpp"
#include "misc/EventQueue.hpp"

class [[nodiscard]] AnimationEngine final
{
private:
    GameScene& scene;
    EventQueue<GameEvent>& eventQueue;
};
