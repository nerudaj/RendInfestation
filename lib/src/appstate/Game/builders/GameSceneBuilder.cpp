#include "appstate/Game/builders/GameSceneBuilder.hpp"
#include "appstate/Game/builders/ActorBuilder.hpp"
#include "appstate/Game/builders/WeaponBuilder.hpp"

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

static inline bool isAbyssTile(int id)
{
    return 31 <= id && id <= 34;
}

const int TRANSPARENT_TILE_ID = 28;
const int WALL_LIGHT_TILE_ID = 38;
const int DOOR_TILE_ID = 42;
const int ENEMY_SPAWN_TILE_ID = 43;
const int PLAYER_SPAWN_TILE_ID = 44;

static inline bool shouldDecorTileBeTransparent(int tile)
{
    return tile - 1 == DOOR_TILE_ID || tile - 1 == PLAYER_SPAWN_TILE_ID
           || tile == 0;
}

GameScene GameSceneBuilder::createScene(
    const GameTextureAtlas& atlas,
    const dgm::ResourceManager& resmgr,
    Input& input,
    const GameModeProperties& gameModeProperties)
{
    const auto level = fetchLevel(resmgr, gameModeProperties);

    auto&& tilesClip = atlas.atlas.getClip(atlas.tilesLocation);
    entt::registry actors;

    for (auto&& prop : level.objectLayer.objects)
    {
        const auto propId = prop.gid - tilesClip.getFrameCount() - 1;
        std::ignore =
            ActorBuilder::createProp(actors, { prop.x, prop.y }, propId, atlas);
    }

    auto artifact = evaluateTileLayers(level, actors, atlas, input);

    // Initialize unlocked modules based on game mode
    if (gameModeProperties.mode == GameMode::Story)
    {
        artifact.unlockedModules = {
            WeaponModule::SpreadBarrel_x2, WeaponModule::SpreadBarrel_x4,
            WeaponModule::CadenceBarrel,   WeaponModule::ExplosiveAmmo,
            WeaponModule::Ricochet,        WeaponModule::PassthruAmmo,
            WeaponModule::BigBullet,       WeaponModule::Spikes,
            WeaponModule::Splitter,
        };
    }
    else
    { // Survival
        artifact.unlockedModules = {};
    }

    return GameScene {
        .actors = std::move(actors),
        .playerEntity = artifact.playerEntity,
        .levelMesh = dgm::Mesh(
            level.meshLayer.data
                | std::views::transform(
                    [](int tile)
                    {
                        return isPassableTile(tile - 1) ? -(tile - 1)
                               : shouldDecorTileBeTransparent(tile)
                                   ? TRANSPARENT_TILE_ID
                                   : tile - 1;
                    })
                | uni::ranges::to<std::vector>(),
            level.dataSize,
            level.voxelSize),
        .decorationsMesh = dgm::Mesh(
            level.decorLayer.data
                | std::views::transform(
                    [](int tile)
                    {
                        return shouldDecorTileBeTransparent(tile)
                                   ? TRANSPARENT_TILE_ID
                                   : tile - 1;
                    })
                | uni::ranges::to<std::vector>(),
            level.dataSize,
            level.voxelSize),
        .altLevelMesh = dgm::Mesh(
            level.meshLayer.data
                | std::views::transform(
                    [](int tile)
                    {
                        return isPassableTileAlt(tile - 1)
                                   ? isAbyssTile(tile - 1) ? -(tile - 1) : 0
                                   : (tile - 1);
                    })
                | uni::ranges::to<std::vector>(),
            level.dataSize,
            level.voxelSize),
        .levelBounds = dgm::Rect(
            { 0.f, 0.f },
            sf::Vector2f(level.dataSize.componentWiseMul(level.voxelSize))),
        .enemySpawns = std::move(artifact.enemySpawns),
        .lights = std::move(artifact.lights),
        .loadout = std::move(artifact.loadout),
        .unlockedModules = std::move(artifact.unlockedModules),
        .survivalSpawnerContext =
            SurvivalSpawnerContext {
                .wave = gameModeProperties.mode == GameMode::Survival ? 0 : -1,
                .enemiesInCurrentWave =
                    gameModeProperties.mode == GameMode::Survival ? 5 : 1000000,
            },
    };
}

GameSceneBuilder::Level GameSceneBuilder::fetchLevel(
    const dgm::ResourceManager& resmgr,
    const GameModeProperties& gameModeProperties)
{
    auto& tiledLevel =
        resmgr.get<tiled::FiniteMapModel>(gameModeProperties.mapName);
    assert(tiledLevel.layers.size() == 3);
    assert(std::holds_alternative<tiled::TileLayerModel>(tiledLevel.layers[0]));
    assert(
        std::holds_alternative<tiled::ObjectGroupModel>(tiledLevel.layers[1]));
    assert(std::holds_alternative<tiled::TileLayerModel>(tiledLevel.layers[2]));
    auto&& layer = std::get<tiled::TileLayerModel>(tiledLevel.layers[0]);
    auto&& decorLayer = std::get<tiled::TileLayerModel>(tiledLevel.layers[2]);

    const auto levelVoxelSize =
        sf::Vector2u { tiledLevel.tilewidth, tiledLevel.tileheight };
    const auto levelDataSize = sf::Vector2u { layer.width, layer.height };

    return Level {
        .meshLayer = layer,
        .objectLayer = std::get<tiled::ObjectGroupModel>(tiledLevel.layers[1]),
        .decorLayer = decorLayer,
        .dataSize = levelDataSize,
        .voxelSize = levelVoxelSize,
    };
}

GameSceneBuilder::LevelCreationArtifact GameSceneBuilder::evaluateTileLayers(
    const Level& level,
    entt::registry& actors,
    const GameTextureAtlas& atlas,
    Input& input)
{
    LevelCreationArtifact result;

    auto tileCoordToWorld = [&](unsigned x, unsigned y)
    {
        return sf::Vector2f {
            x * level.voxelSize.x + level.voxelSize.x / 2.f,
            y * level.voxelSize.y + level.voxelSize.y / 2.f,
        };
    };

    for (unsigned idx = 0, y = 0; y < level.dataSize.y; ++y)
    {
        for (unsigned x = 0; x < level.dataSize.x; ++x, ++idx)
        {
            if (level.meshLayer.data[idx] - 1 == WALL_LIGHT_TILE_ID)
            {
                const auto position =
                    sf::Vector2u { x, y }.componentWiseMul(level.voxelSize)
                    + sf::Vector2u { 16, 69 };
                result.lights.push_back(LightSource {
                    .position = sf::Vector2f { position },
                    .spriteId = 0,
                    .color = COLOR_MUTED_YELLOW,
                });
            }

            if (level.decorLayer.data[idx] - 1 == ENEMY_SPAWN_TILE_ID)
            {
                result.enemySpawns.push_back(tileCoordToWorld(x, y));
            }
            else if (level.decorLayer.data[idx] - 1 == PLAYER_SPAWN_TILE_ID)
            {
                if (result.playerEntity != entt::null)
                {
                    throw std::runtime_error(
                        "Multiple player spawn points in level");
                }

                result.playerEntity = ActorBuilder::createPlayer(
                    actors,
                    tileCoordToWorld(x, y),
                    atlas,
                    input,
                    result.loadout);
            }
            else if (
                level.decorLayer.data[idx] - 1 == DOOR_TILE_ID
                && level.decorLayer.data[idx + 1] - 1 == DOOR_TILE_ID
                && level.decorLayer.data[idx + level.dataSize.x] - 1
                       == DOOR_TILE_ID) // door horizontal
            {
                ActorBuilder::createDoor(
                    actors,
                    sf::Vector2f {
                        sf::Vector2u { x * level.voxelSize.x,
                                       y * level.voxelSize.y + 11 },
                    },
                    atlas);
            }
        }
    }

    return result;
}
