#include "appstate/Game/Janitor.hpp"

void Janitor::cleanScene(GameScene& scene)
{
    for (auto&& entity : objectsToClean)
    {
        if (entity != scene.playerEntity) scene.actors.destroy(entity);
    }

    objectsToClean.clear();
}
