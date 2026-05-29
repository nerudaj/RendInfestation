#pragma once

#include "appstate/Game/definitions/GameScene.hpp"
#include "misc/DependencyContainer.hpp"
#include "settings/AppSettings.hpp"
#include <DGM/dgm.hpp>

class [[nodiscard]] AppStatePause final : public dgm::AppState
{
public:
    AppStatePause(
        dgm::App& app, DependencyContainer& dic, GameScene& scene) noexcept
        : dgm::AppState(
              app,
              dgm::AppStateConfig {
                  .shouldDrawUnderlyingState = true,
              })
        , dic(dic)
        , scene(scene)
    {
        buildLayout();
    }

public:
    void input() override;
    void update() override;
    void draw() override;

private:
    void buildLayout();
    void onResume();
    void onOptions();
    void onBackToMenu();
    void onExit();
    void restoreFocusImpl(const std::string& msg) override;

private:
    DependencyContainer& dic;
    GameScene& scene;
};
