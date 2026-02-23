#include "game/definitions/GameTextureAtlas.hpp"
#include <DGM/classes/ResourceManager.hpp>

class GameTextureAtlasBuilder final
{
public:
    static GameTextureAtlas createTextureAtlas(
        const dgm::ResourceManager& resmgr, const sf::Vector2i& atlasSize)
    {
        auto atlas = dgm::TextureAtlas(atlasSize.x, atlasSize.y);
        auto tiles = atlas
                         .addTileset(
                             resmgr.get<sf::Texture>("scifi_tiles.png"),
                             resmgr.get<dgm::Clip>("scifi_tiles.png.clip"))
                         .value();
        auto explosion =
            atlas
                .addSpritesheet(
                    resmgr.get<sf::Texture>("explosion.png"),
                    resmgr.get<dgm::AnimationStates>("explosion.png.anim"))
                .value();
        auto bullet =
            atlas
                .addSpritesheet(
                    resmgr.get<sf::Texture>("bullet.png"),
                    resmgr.get<dgm::AnimationStates>("bullet.png.anim"))
                .value();
        auto props =
            atlas
                .addSpritesheet(
                    resmgr.get<sf::Texture>("scifi_props.png"),
                    resmgr.get<dgm::AnimationStates>("scifi_props.png.anim"))
                .value();
        auto player =
            atlas
                .addSpritesheet(
                    resmgr.get<sf::Texture>("scifi_soldier.png"),
                    resmgr.get<dgm::AnimationStates>("scifi_soldier.png.anim"))
                .value();
        auto bighead =
            atlas
                .addSpritesheet(
                    resmgr.get<sf::Texture>("scifi_bighead.png"),
                    resmgr.get<dgm::AnimationStates>("scifi_bighead.png.anim"))
                .value();

        return GameTextureAtlas {
            .atlas = std::move(atlas),
            .bulletLocation = bullet,
            .explosionLocation = explosion,
            .playerLocation = player,
            .bigheadLocation = bighead,
            .propsLocation = props,
            .tilesLocation = tiles,
        };
    }
};
