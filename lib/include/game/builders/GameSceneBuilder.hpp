#include "filesystem/models/TiledModels.hpp"
#include "game/definitions/GameScene.hpp"
#include "game/definitions/GameTextureAtlas.hpp"
#include "input/Input.hpp"
#include "misc/Compatibility.hpp"
#include "types/SemanticTypes.hpp"
#include <DGM/classes/ResourceManager.hpp>

class GameSceneBuilder final
{
public:
    static NpcInventory createNpcInventory();

    static GameScene createScene(
        const GameTextureAtlas& atlas,
        const dgm::ResourceManager& resmgr,
        Input& input);
};
