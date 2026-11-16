#pragma once

#include "appstate/Game/definitions/GameScene.hpp"
#include "misc/DependencyContainer.hpp"
#include <DGM/classes/AppState.hpp>

class [[nodiscard]] AppStateUnlocksSummary final : public dgm::AppState
{
public:
    AppStateUnlocksSummary(
        dgm::App& app, DependencyContainer& dic, const Ledger& ledger);

    ~AppStateUnlocksSummary();

public:
    void input() override;

    void update() override;

    void draw() override;

private:
    void buildLayout(const Ledger& ledger);

    tgui::Container::Ptr buildContentLayout(const Ledger& ledger);

private:
    DependencyContainer& dic;
};
