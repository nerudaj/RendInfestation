#include "game/Janitor.hpp"

void Janitor::cleanScene(GameScene& scene)
{
    for (auto&& entity : objectsToClean)
    {
        scene.actors.destroy(entity);
    }

    objectsToClean.clear();
}
