#include "game/Janitor.hpp"

void Janitor::cleanScene(GameScene& scene)
{
    for (auto&& idx : objectsToClean)
    {
        if (scene.actors[idx].inventoryIdx)
            scene.inventories.eraseAtIndex(*scene.actors[idx].inventoryIdx);

        std::visit(
            [&](const auto& collider)
            { scene.actors.eraseAtIndex(idx, collider); },
            scene.actors[idx].body.shape);
    }

    objectsToClean.clear();
}
