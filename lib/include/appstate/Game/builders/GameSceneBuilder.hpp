#include "filesystem/models/TiledModels.hpp"
#include "appstate/Game/definitions/Components.hpp"
#include "appstate/Game/definitions/GameMode.hpp"
#include "appstate/Game/definitions/GameScene.hpp"
#include "appstate/Game/definitions/GameTextureAtlas.hpp"
#include "input/Input.hpp"
#include "misc/Compatibility.hpp"
#include "types/SemanticTypes.hpp"
#include <DGM/classes/ResourceManager.hpp>

class GameSceneBuilder final
{
public:
    static GameScene createScene(
        const GameTextureAtlas& atlas,
        const dgm::ResourceManager& resmgr,
        Input& input,
        const GameModeProperties& gameModeProperties);

private:
    struct [[nodiscard]] Level final
    {
        const tiled::TileLayerModel& meshLayer;
        const tiled::ObjectGroupModel& objectLayer;
        const tiled::TileLayerModel& decorLayer;
        const sf::Vector2u dataSize;
        const sf::Vector2u voxelSize;
    };

    struct [[nodiscard]] LevelCreationArtifact final
    {
        entt::entity playerEntity = entt::null;
        std::vector<LightSource> lights = {};
        std::vector<sf::Vector2f> enemySpawns = {};
        WeaponLoadout loadout = {};
    };

private:
    static Level fetchLevel(
        const dgm::ResourceManager& resmgr,
        const GameModeProperties& gameModeProperties);

    static LevelCreationArtifact evaluateTileLayers(
        const Level& level,
        entt::registry& actors,
        const GameTextureAtlas& atlas,
        Input& input);
};
