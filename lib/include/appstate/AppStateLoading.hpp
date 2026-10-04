#pragma once

#include "appstate/Game/definitions/GameMode.hpp"
#include "appstate/Game/definitions/GameScene.hpp"
#include "appstate/Game/definitions/GameTextureAtlas.hpp"
#include "misc/DependencyContainer.hpp"
#include <DGM/classes/AppState.hpp>
#include <optional>

class [[nodiscard]] AppStateLoading final : public dgm::AppState
{
public:
    AppStateLoading(
        dgm::App& app,
        DependencyContainer& dic,
        const GameModeProperties& gameProps);

public:
    void input() override;

    void update() override;

    void draw() override;

private:
    void restoreFocusImpl(const std::string& message) override;

    void buildLayout();

private:
    DependencyContainer& dic;
    GameModeProperties gameProps;
    bool firstPass = true;
    std::optional<GameTextureAtlas> textureAtlas = std::nullopt;
    std::optional<GameScene> gameScene = std::nullopt;
};
