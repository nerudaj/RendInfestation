#include "game/definitions/GameTextureAtlas.hpp"
#include <DGM/classes/ResourceManager.hpp>

class GameTextureAtlasBuilder final
{
public:
    static GameTextureAtlas createTextureAtlas(
        const dgm::ResourceManager& resmgr, const sf::Vector2i& atlasSize)
    {
        auto atlas = dgm::TextureAtlas(atlasSize.x, atlasSize.y);
        auto mrguyLocation =
            atlas
                .addSpritesheet(
                    resmgr.get<sf::Texture>("mrman.png"),
                    resmgr.get<dgm::AnimationStates>("mrman.png.anim"))
                .value();
        auto tilesLocation = atlas
                                 .addTileset(
                                     resmgr.get<sf::Texture>("tiles.png"),
                                     resmgr.get<dgm::Clip>("tiles.png.clip"))
                                 .value();

        return GameTextureAtlas {
            .atlas = std::move(atlas),
            .mrguyAnimationsLocation = mrguyLocation,
            .tilesLocation = tilesLocation,
        };
    }
};
