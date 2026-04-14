#pragma once

#include "game/definitions/GameScene.hpp"
#include "misc/DependencyContainer.hpp"
#include "settings/AppSettings.hpp"
#include <DGM/dgm.hpp>

class [[nodiscard]] AppStateSurvivalGameOver final : public dgm::AppState
{
public:
    AppStateSurvivalGameOver(
        dgm::App& app,
        DependencyContainer& dic,
        const GameStatus& status) noexcept
        : dgm::AppState(app), dic(dic), status(status)
    {
        buildLayout();
    }

public:
    void input() override;
    void update() override;
    void draw() override;

private:
    void buildLayout();
    tgui::Container::Ptr buildContent();
    void onBackToMenu();

private:
    DependencyContainer& dic;
    GameStatus status;
};
