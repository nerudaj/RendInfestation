#include "game/builders/GameSceneBuilder.hpp"
#include "game/builders/ActorBuilder.hpp"
#include "game/builders/WeaponBuilder.hpp"

static inline bool isPassableTile(int id)
{
    return (0 <= id && id <= 2) || (6 <= id && id <= 8)
           || (12 <= id && id <= 14) || id == 18 || id == 19
           || (24 <= id && id <= 27) || id == 30;
}

static inline bool isPassableTileAlt(int id)
{
    return isPassableTile(id) || (31 <= id && id <= 34);
}

NpcInventory GameSceneBuilder::createNpcInventory()
{
    return NpcInventory {
        .health = 100,
        .damage = 10,
    };
}

const int WALL_LIGHT_TILE_ID = 38;
const int DOOR_TILE_ID = 42;

GameScene GameSceneBuilder::createScene(
    const GameTextureAtlas& atlas,
    const dgm::ResourceManager& resmgr,
    Input& input)
{
    auto tiledLevel = resmgr.get<tiled::FiniteMapModel>("demo-01.json");
    // assert(tiledLevel.layers.size() == 2);
    assert(std::holds_alternative<tiled::TileLayerModel>(tiledLevel.layers[0]));
    assert(
        std::holds_alternative<tiled::ObjectGroupModel>(tiledLevel.layers[1]));
    assert(std::holds_alternative<tiled::TileLayerModel>(tiledLevel.layers[2]));
    auto&& layer = std::get<tiled::TileLayerModel>(tiledLevel.layers[0]);
    auto&& decorLayer = std::get<tiled::TileLayerModel>(tiledLevel.layers[2]);

    const auto levelVoxelSize =
        sf::Vector2u { tiledLevel.tilewidth, tiledLevel.tileheight };
    const auto levelDataSize = sf::Vector2u { layer.width, layer.height };

    auto&& tilesClip = atlas.atlas.getClip(atlas.tilesLocation);
    entt::registry actors;

    const WeaponLoadout loadout {
        .weapon1Modules = { WeaponModule::CadenceBarrel,
                            WeaponModule::ExplosiveAmmo,
                            WeaponModule::None },
        .weapon2Modules = { WeaponModule::SpreadBarrel,
                            WeaponModule::Spikes,
                            WeaponModule::None },
    };

    auto playerEntity = ActorBuilder::createPlayer(
        actors, { 100.f, 150.f }, atlas, input, loadout);

    auto lights = std::vector<LightSource>();

    for (auto&& prop :
         std::get<tiled::ObjectGroupModel>(tiledLevel.layers[1]).objects)
    {
        const auto propId = prop.gid - tilesClip.getFrameCount() - 1;

        auto entity =
            ActorBuilder::createProp(actors, { prop.x, prop.y }, propId, atlas);

        if (propId == 0 || propId == 1)
        {
            lights.push_back(LightSource {
                .position = actors.get<Collider>(entity).getPosition(),
                .spriteId = 7,
                .color = sf::Color::Green,
            });
        }
    }

    for (unsigned idx = 0, y = 0; y < levelDataSize.y; ++y)
    {
        for (unsigned x = 0; x < levelDataSize.x; ++x, ++idx)
        {
            if (layer.data[idx] - 1 == WALL_LIGHT_TILE_ID)
            {
                const auto position =
                    sf::Vector2u { x, y }.componentWiseMul(levelVoxelSize)
                    + sf::Vector2u { 16, 69 };
                lights.push_back(LightSource {
                    .position = sf::Vector2f { position },
                    .spriteId = 0,
                    .color = sf::Color::Yellow,
                });
            }

            if (decorLayer.data[idx] - 1 == DOOR_TILE_ID
                && decorLayer.data[idx + 1] - 1 == DOOR_TILE_ID
                && decorLayer.data[idx + levelDataSize.x] - 1
                       == DOOR_TILE_ID) // door horizontal
            {
                ActorBuilder::createDoor(
                    actors,
                    sf::Vector2f {
                        sf::Vector2u { x * levelVoxelSize.x,
                                       y * levelVoxelSize.y + 11 },
                    },
                    atlas);
            }
        }
    }

    return GameScene {
        .actors = std::move(actors),
        .playerEntity = playerEntity,
        .levelMesh = dgm::Mesh(
            layer.data
                | std::views::transform(
                    [](int tile)
                    {
                        return isPassableTile(tile - 1) ? -(tile - 1)
                                                        : (tile - 1);
                    })
                | uniranges::to<std::vector>(),
            levelDataSize,
            levelVoxelSize),
        .decorationsMesh = dgm::Mesh(
            decorLayer.data
                | std::views::transform(
                    [](int tile)
                    { return tile - 1 == DOOR_TILE_ID ? 28 : tile - 1; })
                | uniranges::to<std::vector>(),
            levelDataSize,
            levelVoxelSize),
        .altLevelMesh = dgm::Mesh(
            layer.data
                | std::views::transform(
                    [](int tile)
                    {
                        return isPassableTileAlt(tile - 1) ? -(tile - 1)
                                                           : (tile - 1);
                    })
                | uniranges::to<std::vector>(),
            levelDataSize,
            levelVoxelSize),
        .levelBounds = dgm::Rect(
            { 0.f, 0.f },
            sf::Vector2f(levelDataSize.componentWiseMul(levelVoxelSize))),
        .enemySpawns =
            std::vector<sf::Vector2f> {
                { 1006.f, 109.f },
                { 1808.f, 170.f },
                { 80.f, 681.f },
                { 1846, 703.f },
                { 1471.f, 1116.f },
            },
        .lights = std::move(lights),
        .loadout = loadout,
    };
}
