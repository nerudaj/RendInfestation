#include "game/builders/GameSceneBuilder.hpp"
#include "game/builders/ActorBuilder.hpp"
#include "game/builders/WeaponBuilder.hpp"

static inline bool isPassableTile(int id)
{
    return (0 <= id && id <= 2) || (6 <= id && id <= 8)
           || (12 <= id && id <= 14) || id == 18 || id == 19
           || (24 <= id && id <= 27) || id == 30;
}

PlayerInventory GameSceneBuilder::createPlayerInventory()
{
    return PlayerInventory {
        .health = 100,
        .weapons =
            std::array {
                WeaponBuilder::createWeapon({ WeaponModule::CadenceBarrel,
                                              WeaponModule::SpreadBarrel,
                                              WeaponModule::ExplosiveAmmo }),
                WeaponBuilder::createWeapon({ WeaponModule::None }),
            },
    };
}

NpcInventory GameSceneBuilder::createNpcInventory()
{
    return NpcInventory {
        .health = 100,
    };
}

GameScene GameSceneBuilder::createScene(
    const GameTextureAtlas& atlas, const dgm::ResourceManager& resmgr)
{
    auto tiledLevel = resmgr.get<tiled::FiniteMapModel>("demo-01.json");
    assert(tiledLevel.layers.size() == 2);
    assert(std::holds_alternative<tiled::TileLayerModel>(tiledLevel.layers[0]));
    assert(
        std::holds_alternative<tiled::ObjectGroupModel>(tiledLevel.layers[1]));
    auto&& layer = std::get<tiled::TileLayerModel>(tiledLevel.layers[0]);

    const auto levelVoxelSize =
        sf::Vector2u { tiledLevel.tilewidth, tiledLevel.tileheight };
    const auto levelDataSize = sf::Vector2u { layer.width, layer.height };

    auto&& tilesClip = atlas.atlas.getClip(atlas.tilesLocation);
    auto actors = dgm::DynamicBuffer<Actor>();

    auto inventories = dgm::DynamicBuffer<Inventory>();
    actors.emplaceBack(ActorBuilder::createPlayer(
        { 100.f, 150.f },
        atlas,
        inventories.emplaceBack(createPlayerInventory())));

    for (auto&& prop :
         std::get<tiled::ObjectGroupModel>(tiledLevel.layers[1]).objects)
    {
        const auto propId = prop.gid - tilesClip.getFrameCount() - 1;

        actors.emplaceBack(
            ActorBuilder::createProp({ prop.x, prop.y }, propId, atlas));
    }

    return GameScene {
        .actors = std::move(actors),
        .inventories = std::move(inventories),
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
    };
}
