#include "game/builders/GameTextureAtlasBuilder.hpp"
#include <DGM/classes/ResourceManager.hpp>

GameTextureAtlas GameTextureAtlasBuilder::createTextureAtlas(
    const dgm::ResourceManager& resmgr, const sf::Vector2i& atlasSize)
{
    auto atlas = dgm::TextureAtlas(atlasSize.x, atlasSize.y);

    auto props =
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("scifi_props.png"),
                resmgr.get<dgm::AnimationStates>("scifi_props.png.anim"))
            .value();

    auto lights = atlas
                      .addTileset(
                          resmgr.get<sf::Texture>("lights.png"),
                          resmgr.get<dgm::Clip>("lights.png.clip"))
                      .value();

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
    auto bullet = atlas
                      .addSpritesheet(
                          resmgr.get<sf::Texture>("bullet.png"),
                          resmgr.get<dgm::AnimationStates>("bullet.png.anim"))
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

    auto scuttlebug =
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("scuttlebug.png"),
                resmgr.get<dgm::AnimationStates>("scuttlebug.png.anim"))
            .value();

    auto smallBullet =
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("projectile_small_bullet.png"),
                resmgr.get<dgm::AnimationStates>(
                    "projectile_small_bullet.png.anim"))
            .value();
    auto landmine = atlas
                        .addSpritesheet(
                            resmgr.get<sf::Texture>("projectile_landmine.png"),
                            resmgr.get<dgm::AnimationStates>(
                                "projectile_landmine.png.anim"))
                        .value();
    auto spikes =
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("projectile_spikes.png"),
                resmgr.get<dgm::AnimationStates>("projectile_spikes.png.anim"))
            .value();
    auto hyperbeam =
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("projectile_hyperbeam.png"),
                resmgr.get<dgm::AnimationStates>(
                    "projectile_hyperbeam.png.anim"))
            .value();

    auto crosshairs = atlas
                          .addTileset(
                              resmgr.get<sf::Texture>("crosshairs.png"),
                              resmgr.get<dgm::Clip>("crosshairs.png.clip"))
                          .value();

    auto doorHorizontal =
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("scifi_door.png"),
                resmgr.get<dgm::AnimationStates>("scifi_door.png.anim"))
            .value();

    auto hud = atlas
                   .addTileset(
                       resmgr.get<sf::Texture>("infestation_hud.png"),
                       resmgr.get<dgm::Clip>("infestation_hud.png.clip"))
                   .value();

    auto icons = atlas
                     .addTileset(
                         resmgr.get<sf::Texture>("pixel-ui-icons.png"),
                         resmgr.get<dgm::Clip>("pixel-ui-icons.png.clip"))
                     .value();

    auto modules =
        atlas
            .addTileset(
                resmgr.get<sf::Texture>("infestation_modules.png"),
                resmgr.get<dgm::Clip>("infestation_modules.png.clip"))
            .value();

    auto scuttlebugBlue =
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("scuttlebug_blue.png"),
                resmgr.get<dgm::AnimationStates>("scuttlebug_blue.png.anim"))
            .value();

    auto beholder =
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("beholder.png"),
                resmgr.get<dgm::AnimationStates>("beholder.png.anim"))
            .value();

    auto image = sf::Image(atlas.getTexture().copyToImage());
    std::ignore = image.saveToFile("atlas_debug.png");

    return GameTextureAtlas {
        .atlas = std::move(atlas),
        .smallBulletLocation = smallBullet,
        .bulletLocation = bullet,
        .landmineLocation = landmine,
        .spikesLocation = spikes,
        .hyperbeamLocation = hyperbeam,
        .explosionLocation = explosion,
        .playerLocation = player,
        .bigheadLocation = bighead,
        .scuttlebugLocation = scuttlebug,
        .scuttlebugBlueLocation = scuttlebugBlue,
        .beholderLocation = beholder,
        .propsLocation = props,
        .doorHorizontalLocation = doorHorizontal,
        .tilesLocation = tiles,
        .crosshairsLocation = crosshairs,
        .lightsLocation = lights,
        .hudLocation = hud,
        .iconsLocation = icons,
        .modulesLocation = modules,
    };
}
