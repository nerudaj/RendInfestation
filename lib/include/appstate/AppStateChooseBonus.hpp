#pragma once

#include "appstate/Game/definitions/GameScene.hpp"
#include "misc/DependencyContainer.hpp"
#include "settings/AppSettings.hpp"
#include <DGM/dgm.hpp>
#include <utility>

class [[nodiscard]] AppStateChooseBonus final : public dgm::AppState
{
public:
    AppStateChooseBonus(
        dgm::App& app,
        DependencyContainer& dic,
        GameScene& scene,
        const bool allowGoldModules) noexcept
        : dgm::AppState(
              app,
              dgm::AppStateConfig {
                  .shouldDrawUnderlyingState = true,
              })
        , dic(dic)
        , scene(scene)
        , generatedModules(generatePickerSelection(allowGoldModules))
    {
        buildLayout();
    }

public:
    void input() override;
    void update() override;
    void draw() override;

protected:
    void restoreFocusImpl(const std::string& msg) override;

private:
    static tgui::Layout2d getCoreLayoutSize();
    void buildLayout();
    tgui::Panel::Ptr createSelectablePanel(tgui::Layout2d size);
    tgui::Container::Ptr createPickerBox(WeaponModule module);
    std::pair<WeaponModule, WeaponModule>
    generatePickerSelection(const bool allowGoldModues);
    void onSubmit(WeaponModule module);

private:
    DependencyContainer& dic;
    GameScene& scene;
    std::pair<WeaponModule, WeaponModule> generatedModules;
};
