#include "appstate/Game/definitions/GameScene.hpp"
#include "appstate/Game/builders/WeaponBuilder.hpp"

void GameScene::updatePlayerLoadout()
{
    auto& inventory = actors.get<WeaponInventory>(playerEntity);

    inventory.weapons =
        loadout.weapons
        | uni::views::transform(
            [](const WeaponConfig& config)
            { return WeaponBuilder::createWeapon(EntityKind::Player, config); })
        | uni::ranges::to<std::vector>();
}
