#pragma once

#include "misc/DependencyContainer.hpp"
#include <DGM/classes/AppState.hpp>

class [[nodiscard]] AppStateAreYouSure final : public dgm::AppState
{
public:
    AppStateAreYouSure(
        dgm::App& app,
        DependencyContainer& dic,
        StringId descriptionStringId,
        bool popOnConfirm,
        std::function<void(void)> onConfirm);

    ~AppStateAreYouSure();

public:
    void input() override;

    void update() override;

    void draw() override;

private:
    void buildLayout(StringId descriptionStringId, bool popOnConfirm);

    tgui::Container::Ptr buildContentLayout(StringId descriptionStringId);

    void onCancel();

private:
    DependencyContainer& dic;
    std::function<void(void)> onConfirm;
};
