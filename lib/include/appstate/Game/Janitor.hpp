#pragma once

#include "appstate/Game/definitions/GameEvents.hpp"
#include "appstate/Game/definitions/GameScene.hpp"
#include <entt/entt.hpp>
#include <set>

class [[nodiscard]] Janitor final
{
public:
    void operator()(const event::ObjectDestroyed& e)
    {
        std::cerr << "Object registered for cleanup: "
                  << static_cast<size_t>(e.entity) << std::endl;
        objectsToClean.insert(e.entity);
    }

    void operator()(const auto&) {}

public:
    void cleanScene(GameScene& scene);

private:
    std::set<entt::entity> objectsToClean;
};
