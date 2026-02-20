#include "filesystem/models/TiledModels.hpp"
#include "game/definitions/GameScene.hpp"
#include "game/definitions/GameTextureAtlas.hpp"
#include "misc/Compatibility.hpp"
#include "types/SemanticTypes.hpp"
#include <DGM/classes/ResourceManager.hpp>

class GameSceneBuilder final
{
public:
    static PlayerInventory createPlayerInventory();

    static Actor createPlayer(
        const sf::Vector2f& spawnPosition,
        const GameTextureAtlas& atlas,
        size_t inventoryIdx);

    static NpcInventory createNpcInventory();

    static Actor createNpc(
        const sf::Vector2f& spawnPosition,
        const GameTextureAtlas& atlas,
        size_t inventoryIdx);

    static GameScene createScene(
        const GameTextureAtlas& atlas, const dgm::ResourceManager& resmgr);
};
