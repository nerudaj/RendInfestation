#include "game/definitions/GameTextureAtlas.hpp"
#include <DGM/classes/ResourceManager.hpp>

class GameTextureAtlasBuilder final
{
public:
    static GameTextureAtlas createTextureAtlas(
        const dgm::ResourceManager& resmgr, const sf::Vector2i& atlasSize);
};
