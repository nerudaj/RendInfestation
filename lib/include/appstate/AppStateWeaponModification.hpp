#pragma once

#include "appstate/Game/definitions/GameScene.hpp"
#include "appstate/WeaponModification/AnimationTimer.hpp"
#include "appstate/WeaponModification/Renderer.hpp"
#include "misc/DependencyContainer.hpp"
#include <DGM/dgm.hpp>
#include <string>
#include <vector>

class [[nodiscard]] AppStateWeaponModification final : public dgm::AppState
{
public:
    AppStateWeaponModification(
        dgm::App& app, DependencyContainer& dic, GameScene& scene);

public:
    void input() override;
    void update() override;
    void draw() override;

private:
    void buildLayout();

    tgui::Container::Ptr buildLoadoutPanel(size_t loadoutIdx);

    tgui::Container::Ptr buildButtonForSelectingModule(
        const WeaponModule module, size_t moduleIdx, size_t loadoutIdx);

    std::array<WeaponModule, 3>& getCurrentLoadout()
    {
        return scene.loadout.weapons[currentWeaponIdx].modules;
    }

    const std::array<WeaponModule, 3>& getCurrentLoadout() const
    {
        return scene.loadout.weapons[currentWeaponIdx].modules;
    }

    const tgui::Texture& getWeaponModuleTexture(const WeaponModule module) const
    {
        return dic.resmgr.get<tgui::Texture>(
            uni::format("ModuleIcon-{}", std::to_underlying(module)));
    }

    void onResume();
    void onBack();
    void onModSelected();
    void onClearLoadout(size_t loadoutIdx);

    void restoreGuiViewport();

    void restoreFocusImpl(const std::string&) override;

private:
    DependencyContainer& dic;
    GameScene& scene;

    int currentWeaponIdx = 0;

    size_t selectedLoadoutIdx = 0;
    size_t selectedModuleIdx = 0;
    std::optional<WeaponModule> selectedModule = std::nullopt;
};
