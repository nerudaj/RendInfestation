#pragma once

#include "appstate/Game/definitions/GameScene.hpp"
#include "misc/DependencyContainer.hpp"
#include <DGM/classes/AppState.hpp>
#include <optional>
#include <ranges>

class [[nodiscard]] AppStateWeaponModuleSelection final : public dgm::AppState
{
public:
    AppStateWeaponModuleSelection(
        dgm::App& app,
        DependencyContainer& dic,
        const GameScene& scene,
        std::optional<WeaponModule>& outModule,
        const WeaponModule currentlySelectedModule);

    ~AppStateWeaponModuleSelection();

public:
    void input() override;

    void update() override;

    void draw() override;

private:
    tgui::ChildWindow::Ptr
    createModuleSelectModal(StringId titleStringId, tgui::Layout2d size) const;

    void buildLayout();

    tgui::Container::Ptr buildModuleSelectionLayout();

    tgui::Container::Ptr buildModuleDetailLayout();

    tgui::Container::Ptr buildModuleListEntryLayout(const WeaponModule module);

    void refreshModuleDetailPanel();

    const tgui::Texture& getWeaponModuleTexture(const WeaponModule module) const
    {
        return dic.resmgr.get<tgui::Texture>(
            uni::format("ModuleIcon-{}", std::to_underlying(module)));
    }

    std::string getModuleListEntryId(const WeaponModule module) const
    {
        return std::format("ModuleListEntry-{}", std::to_underlying(module));
    }

    bool isInUseModule(const WeaponModule module) const
    {
        for (auto&& loadout : scene.loadout.weapons)
        {
            if (std::ranges::find(loadout.modules, module)
                != loadout.modules.end())
                return true;
        }

        return false;
    }

    void onCancel();

    void onApply();

    void onModuleSelected(const WeaponModule newSelectedModule);

private:
    DependencyContainer& dic;
    const GameScene& scene;
    std::optional<WeaponModule>& outModule;
    WeaponModule selectedModule;
};
