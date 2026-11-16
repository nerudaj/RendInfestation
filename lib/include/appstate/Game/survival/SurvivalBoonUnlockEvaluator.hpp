#pragma once

#include "appstate/Game/definitions/GameMode.hpp"
#include "appstate/Game/definitions/GameScene.hpp"
#include "misc/SaveManager.hpp"

/*
 * NOTE for future me
 *
 * This probably should've been an "engine" or a "system"
 * in ECS terms.
 */
class [[nodiscard]] SurvivalBoonUnlockEvaluator
{
public:
    SurvivalBoonUnlockEvaluator(SaveManager& saves) noexcept;

public:
    void evaluateUnlocks(
        const int beatenWaveNum,
        const size_t mapIdx,
        const PlayerInventory& playerInventory,
        std::function<void(StringId)>&& postMessage);

private:
    SaveManager& saves;
};
