#pragma once

#include "game/definitions/GameScene.hpp"
#include "game/definitions/NpcBlackboard.hpp"
#include "game/input/NpcInput.hpp"
#include <DGM/classes/Time.hpp>

class [[nodiscard]] AiEngine final
{
public:
    AiEngine(GameScene& _scene)
        : scene(_scene), navMesh(scene.levelMesh.clone())
    {
    }

public:
    void update(const dgm::Time& time);

private:
    void updateNpcBrain(
        NpcInput& input, NpcBlackboard& blackboard, entt::entity npc);

private:
    bool isNpcWalking(entt::entity npc);

    bool hasNpcReachedWaypoint(
        const sf::Vector2f position, const sf::Vector2f target);

private:
    GameScene& scene;
    dgm::WorldNavMesh navMesh;
};
