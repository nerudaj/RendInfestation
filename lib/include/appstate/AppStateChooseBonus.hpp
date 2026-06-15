#pragma once

#include "appstate/Game/definitions/GameScene.hpp"
#include "misc/DependencyContainer.hpp"
#include "settings/AppSettings.hpp"
#include <DGM/dgm.hpp>

class [[nodiscard]] AppStateChooseBonus final : public dgm::AppState
{
public:
    AppStateChooseBonus(
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
    static tgui::Layout2d getCoreLayoutSize();
    void buildLayout();
    tgui::Panel::Ptr createSelectablePanel(tgui::Layout2d size);
    tgui::Container::Ptr createPickerBox(WeaponModule module);
    std::pair<WeaponModule, WeaponModule> generatePickerSelection();
    void onSubmit(WeaponModule module);

private:
    DependencyContainer& dic;
    GameScene& scene;
};
