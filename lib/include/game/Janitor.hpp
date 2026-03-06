#pragma once

#include "game/definitions/GameEvents.hpp"
#include "game/definitions/GameScene.hpp"
#include <entt/entt.hpp>
#include <set>

class [[nodiscard]] Janitor final
{
public:
    void operator()(const event::ObjectDestroyed& e)
    {
        objectsToClean.insert(e.entity);
    }

    void operator()(const auto&) {}

public:
    void cleanScene(GameScene& scene);

private:
    std::set<entt::entity> objectsToClean;
};
