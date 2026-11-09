#pragma once

#include "misc/DependencyContainer.hpp"
#include <DGM/classes/AppState.hpp>

class [[nodiscard]] AppStateNoteReader final : public dgm::AppState
{
public:
    AppStateNoteReader(dgm::App& app, DependencyContainer& dic, int noteId);

public:
    void input() override;

    void update() override;

    void draw() override;

private:
    void buildLayout(int noteId);

private:
    DependencyContainer& dic;
};
