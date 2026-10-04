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
    // TODO: string IDs
    const static inline std::vector<std::string> LEVEL_NAMES = {
        "  Landing pad", "  Botany bay", "  Labs"
    };
    const static inline std::vector<std::string> LEVEL_IDS = {
        "survival-02.json", "survival-03.json", "survival-04.json"
    };

    DependencyContainer& dic;
    GameModeProperties gameProps = {
        .mode = GameMode::Survival,
        .mapName = LEVEL_IDS.front(),
    };
};
