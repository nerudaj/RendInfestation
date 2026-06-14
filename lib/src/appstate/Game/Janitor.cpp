#include "appstate/Game/Janitor.hpp"

void Janitor::cleanScene(GameScene& scene)
{
    for (auto&& entity : objectsToClean)
    {
        std::cerr << "Deleting entity " << static_cast<size_t>(entity)
                  << std::endl;
        scene.actors.destroy(entity);
    }

    objectsToClean.clear();
}
