#include "game/Janitor.hpp"

void Janitor::cleanScene(GameScene& scene)
{
    for (auto&& idx : objectsToClean)
    {
        if (scene.actors[idx].inventoryIdx)
            scene.inventories.eraseAtIndex(*scene.actors[idx].inventoryIdx);

        scene.actors.eraseAtIndex(idx);
    }

    objectsToClean.clear();
}
