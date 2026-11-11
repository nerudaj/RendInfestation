#pragma once

#include "appstate/Game/definitions/GameScene.hpp"
#include "misc/DependencyContainer.hpp"
#include "settings/AppSettings.hpp"
#include <DGM/dgm.hpp>

class [[nodiscard]] AppStateSurvivalGameOver final : public dgm::AppState
{
public:
    AppStateSurvivalGameOver(
        dgm::App& app,
        DependencyContainer& dic,
        const GameStatus& status,
        const size_t mapIdx) noexcept;

public:
    void input() override;
    void update() override;
    void draw() override;

private:
    void buildLayout();
    tgui::Container::Ptr buildBentoBoxContent();
    tgui::Container::Ptr buildGameSummaryLayout();
    tgui::Container::Ptr buildLedgerSummaryLayout();

    void onRestart();
    void onBackToMenu();

private:
    DependencyContainer& dic;
    GameStatus status;
    const size_t mapIdx;
};
