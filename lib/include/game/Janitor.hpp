#pragma once

#include "game/definitions/GameEvents.hpp"
#include "game/definitions/GameScene.hpp"
#include <set>

class [[nodiscard]] Janitor final
{
public:
    void operator()(const event::ObjectDestroyed& e)
    {
        objectsToClean.insert(e.idx);
    }

    void operator()(const auto&) {}

public:
    void cleanScene(GameScene& scene);

private:
    std::set<size_t> objectsToClean;
};
