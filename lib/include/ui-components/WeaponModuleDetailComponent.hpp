#pragma once

#include "appstate/Game/enums/WeaponModule.hpp"
#include "misc/DependencyContainer.hpp"
#include <functional>

class [[nodiscard]] WeaponModuleDetailComponent final
{
public:
    explicit WeaponModuleDetailComponent(
        DependencyContainer& dic,
        std::function<tgui::Texture(const WeaponModule)>
            getWeaponModuleTexture) noexcept
        : dic(dic), getWeaponModuleTexture(std::move(getWeaponModuleTexture))
    {
    }

public:
    tgui::Container::Ptr buildLayout();

    void refreshDetail(const WeaponModule module);

private:
    DependencyContainer& dic;
    std::function<tgui::Texture(const WeaponModule)> getWeaponModuleTexture;
};
