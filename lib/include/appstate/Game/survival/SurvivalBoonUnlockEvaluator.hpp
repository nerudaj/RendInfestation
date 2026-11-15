#pragma once

#include "appstate/Game/definitions/GameMode.hpp"
#include "appstate/Game/definitions/GameScene.hpp"
#include "misc/SaveManager.hpp"

class [[nodiscard]] SurvivalBoonUnlockEvaluator
{
public:
    SurvivalBoonUnlockEvaluator(SaveManager& saves) noexcept;

public:
    void evaluateUnlocks(
        const int beatedWaveNum,
        const int mapIdx,
        const PlayerInventory& playerInventory,
        std::function<void(StringId)>&& postMessage);

private:
    SaveManager& saves;
};
