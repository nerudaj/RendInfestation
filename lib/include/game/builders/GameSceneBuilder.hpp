#include "filesystem/models/TiledModels.hpp"
#include "game/definitions/GameScene.hpp"
#include "game/definitions/GameTextureAtlas.hpp"
#include "misc/Compatibility.hpp"
#include <DGM/classes/ResourceManager.hpp>

class GameSceneBuilder final
{
public:
    static GameScene createScene(
        const GameTextureAtlas& atlas, const dgm::ResourceManager& resmgr)
    {
        auto&& animation = dgm::Animation(
            atlas.atlas.getAnimationStates(atlas.mrguyAnimationsLocation), 4);
        animation.setState("idle", true);

        auto tiledLevel = resmgr.get<tiled::FiniteMapModel>("dummy.json");
        assert(tiledLevel.layers.size() == 1);
        assert(std::holds_alternative<tiled::TileLayerModel>(
            tiledLevel.layers[0]));
        auto&& layer = std::get<tiled::TileLayerModel>(tiledLevel.layers[0]);

        return GameScene {
            .dummy =
                DummyEntity {
                    .body = dgm::Rect({ 100.f, 100.f }, { 32.f, 60.f }),
                    .animation = std::move(animation),
                },
            .levelMesh = dgm::Mesh(
                layer.data
                    | std::views::transform([](int tile) { return tile - 1; })
                    | uniranges::to<std::vector>(),
                { layer.width, layer.height },
                { tiledLevel.tilewidth, tiledLevel.tileheight }),
        };
    }
};
