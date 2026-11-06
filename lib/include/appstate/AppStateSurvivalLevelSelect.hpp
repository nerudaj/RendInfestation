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

    tgui::Container::Ptr buildMapSelectLayout();

    tgui::Container::Ptr buildBoonSelectLayout();

    void buildMapDetailLayout(tgui::Container::Ptr parent, size_t mapIdx);

    void onMapSelected(size_t mapIdx);

    void onBack();

    void onPlay();

private:
    struct [[nodiscard]] LevelContext final
    {
        StringId levelName;
        StringId levelLockedString;
        std::string fileId;
        std::string imageName;
    };

    const static inline auto LEVELS = std::vector<LevelContext> {
        LevelContext {
            .levelName = StringId::SurvivalMap01_Name,
            .levelLockedString = StringId::SurvivalMap01_Locked,
            .fileId = "survival-02.json",
            .imageName = "placeholder16_9.png",
        },
        LevelContext {
            .levelName = StringId::SurvivalMap02_Name,
            .levelLockedString = StringId::SurvivalMap02_Locked,
            .fileId = "survival-03.json",
            .imageName = "placeholder16_9.png",
        },
        LevelContext {
            .levelName = StringId::SurvivalMap03_Name,
            .levelLockedString = StringId::SurvivalMap03_Locked,
            .fileId = "survival-04.json",
            .imageName = "placeholder16_9.png",
        },
    };

    DependencyContainer& dic;
    GameModeProperties gameProps = {
        .mode = GameMode::Survival,
        .mapName = LEVELS.front().fileId,
    };
};
