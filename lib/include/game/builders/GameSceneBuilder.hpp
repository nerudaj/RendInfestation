#include "filesystem/models/TiledModels.hpp"
#include "game/definitions/GameScene.hpp"
#include "game/definitions/GameTextureAtlas.hpp"
#include "misc/Compatibility.hpp"
#include "types/SemanticTypes.hpp"
#include <DGM/classes/ResourceManager.hpp>

static inline bool isPassableTile(int id)
{
    return (0 <= id && id <= 2) || (6 <= id && id <= 8)
           || (12 <= id && id <= 14) || id == 18 || id == 19
           || (24 <= id && id <= 27) || id == 30;
}

class GameSceneBuilder final
{
public:
    static GameScene createScene(
        const GameTextureAtlas& atlas, const dgm::ResourceManager& resmgr)
    {
        auto tiledLevel = resmgr.get<tiled::FiniteMapModel>("demo-01.json");
        assert(tiledLevel.layers.size() == 2);
        assert(std::holds_alternative<tiled::TileLayerModel>(
            tiledLevel.layers[0]));
        assert(std::holds_alternative<tiled::ObjectGroupModel>(
            tiledLevel.layers[1]));
        auto&& layer = std::get<tiled::TileLayerModel>(tiledLevel.layers[0]);

        auto&& propsClip = atlas.atlas.getClip(atlas.propsLocation);
        auto&& tilesClip = atlas.atlas.getClip(atlas.tilesLocation);
        auto props = dgm::DynamicBuffer<Actor>();
        for (auto&& prop :
             std::get<tiled::ObjectGroupModel>(tiledLevel.layers[1]).objects)
        {
            const auto propId = prop.gid - tilesClip.getFrameCount() - 1;

            auto idx = props.emplaceBack(Actor {
                .kind = ActorKind::Prop,
                .body = dgm::Rect({ prop.x, prop.y - 64.f }, { 64.f, 64.f }),
                .animation = dgm::Animation(atlas.propsStates),
            });

            props[idx].animation.setState(
                std::format("idle-{}", propId), "looping"_true);
        }

        auto idx = props.emplaceBack(Actor {
            .kind = ActorKind::Player,
            .body = dgm::Circle({ 100.f, 100.f }, 8.f),
            .animation = dgm::Animation(
                atlas.atlas.getAnimationStates(atlas.playerLocation)),
        });

        props[idx].animation.setState("idle-front", "looping"_true);

        return GameScene {
            .actors = std::move(props),
            .levelMesh = dgm::Mesh(
                layer.data
                    | std::views::transform(
                        [](int tile)
                        {
                            return isPassableTile(tile - 1) ? -(tile - 1)
                                                            : (tile - 1);
                        })
                    | uniranges::to<std::vector>(),
                { layer.width, layer.height },
                { tiledLevel.tilewidth, tiledLevel.tileheight }),
        };
    }
};
