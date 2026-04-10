#include "game/builders/GameTextureAtlasBuilder.hpp"
#include <DGM/classes/ResourceManager.hpp>

GameTextureAtlas GameTextureAtlasBuilder::createTextureAtlas(
    const dgm::ResourceManager& resmgr, const sf::Vector2i& atlasSize)
{
    auto atlas = dgm::TextureAtlas(atlasSize.x, atlasSize.y);

    auto skins = decltype(GameTextureAtlas::skinLocations) {};

    skins.emplace(
        SkinType::Prop,
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("scifi_props.png"),
                resmgr.get<dgm::AnimationStates>("scifi_props.png.anim"))
            .value());

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

    skins.emplace(
        SkinType::Explosion,
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("explosion_1.png"),
                resmgr.get<dgm::AnimationStates>("explosion_1.png.anim"))
            .value());
    skins.emplace(
        SkinType::BigBullet,
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("bullet.png"),
                resmgr.get<dgm::AnimationStates>("bullet.png.anim"))
            .value());

    skins.emplace(
        SkinType::PlayerDefault,
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("scifi_soldier.png"),
                resmgr.get<dgm::AnimationStates>("scifi_soldier.png.anim"))
            .value());
    skins.emplace(
        SkinType::Bighead,
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("scifi_bighead.png"),
                resmgr.get<dgm::AnimationStates>("scifi_bighead.png.anim"))
            .value());

    skins.emplace(
        SkinType::Scuttlebug,
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("scuttlebug.png"),
                resmgr.get<dgm::AnimationStates>("scuttlebug.png.anim"))
            .value());

    skins.emplace(
        SkinType::SmallBullet,
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("projectile_small_bullet.png"),
                resmgr.get<dgm::AnimationStates>(
                    "projectile_small_bullet.png.anim"))
            .value());
    skins.emplace(
        SkinType::Landmine,
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("projectile_landmine.png"),
                resmgr.get<dgm::AnimationStates>(
                    "projectile_landmine.png.anim"))
            .value());
    skins.emplace(
        SkinType::Spikes,
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("projectile_spikes.png"),
                resmgr.get<dgm::AnimationStates>("projectile_spikes.png.anim"))
            .value());
    skins.emplace(
        SkinType::Hyperbeam,
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("projectile_hyperbeam.png"),
                resmgr.get<dgm::AnimationStates>(
                    "projectile_hyperbeam.png.anim"))
            .value());

    auto crosshairs = atlas
                          .addTileset(
                              resmgr.get<sf::Texture>("crosshairs.png"),
                              resmgr.get<dgm::Clip>("crosshairs.png.clip"))
                          .value();

    skins.emplace(
        SkinType::DoorHorizontal,
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("scifi_door.png"),
                resmgr.get<dgm::AnimationStates>("scifi_door.png.anim"))
            .value());

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

    skins.emplace(
        SkinType::ScuttlebugBlue,
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("scuttlebug_blue.png"),
                resmgr.get<dgm::AnimationStates>("scuttlebug_blue.png.anim"))
            .value());

    skins.emplace(
        SkinType::Beholder,
        atlas
            .addSpritesheet(
                resmgr.get<sf::Texture>("beholder.png"),
                resmgr.get<dgm::AnimationStates>("beholder.png.anim"))
            .value());

    auto image = sf::Image(atlas.getTexture().copyToImage());
    std::ignore = image.saveToFile("atlas_debug.png");

    return GameTextureAtlas {
        .atlas = std::move(atlas),
        .skinLocations = std::move(skins),
        .tilesLocation = tiles,
        .crosshairsLocation = crosshairs,
        .lightsLocation = lights,
        .hudLocation = hud,
        .iconsLocation = icons,
        .modulesLocation = modules,
    };
}
