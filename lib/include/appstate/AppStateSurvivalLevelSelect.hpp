#pragma once

#include "appstate/Game/definitions/GameMode.hpp"
#include "misc/DependencyContainer.hpp"
#include <DGM/classes/AppState.hpp>

class [[nodiscard]] AppStateSurvivalLevelSelect final : public dgm::AppState
{
public:
    AppStateSurvivalLevelSelect(dgm::App& app, DependencyContainer& dic);

public:
    void input() override;

    void update() override;

    void draw() override;

private:
    void restoreFocusImpl(const std::string& message) override;

    void buildLayout();

    tgui::Container::Ptr buildContent();

    void onBack();

    void onPlay();

private:
    DependencyContainer& dic;
    GameModeProperties gameProps = {};
};
