#include "game/engine/AiEngine.hpp"

void AiEngine::update(const dgm::Time& time)
{
    // loop over all npcs (they have NpcInputs)
    // throw their logic into some FSM
    // update input

    for (auto&& [entity, blackboard, input] :
         scene.actors.view<NpcBlackboard, EntityInput>().each())
    {
        updateNpcBrain(
            dynamic_cast<NpcInput&>(*input.get()), blackboard, entity);
    }
}

void AiEngine::updateNpcBrain(
    NpcInput& input, NpcBlackboard& blackboard, entt::entity npc)
{
    input.clearInputs();

    const auto position = scene.actors.get<Collider>(npc).getPosition();
    if (blackboard.targetEntity == entt::null)
    {
        blackboard.targetEntity = scene.playerEntity;
        blackboard.movementTarget = position;
    }

    const auto targetEntityPosition =
        scene.actors.get<Collider>(blackboard.targetEntity).getPosition();
    const auto directionToTarget = targetEntityPosition - position;
    const bool isWalking = isNpcWalking(npc);

    if (isWalking && directionToTarget.length() <= 20.f)
    {
        input.setShooting(true);
    }
    if (isWalking
        && !hasNpcReachedWaypoint(position, blackboard.movementTarget))
    {
        input.setForward(
            [&]
            {
                if (!isWalking) return sf::Vector2f { 0.f, 0.f };
                return dgm::Math::toUnit(blackboard.movementTarget - position);
            }());
    }
    else
    {
        auto path = navMesh.computePath(position, targetEntityPosition);

        if (!path.isTraversed())
            blackboard.movementTarget = path.getCurrentPoint().coord;
    }

    input.setAimDirection(dgm::Math::toUnit(directionToTarget));
}

bool AiEngine::isNpcWalking(entt::entity npc)
{
    const auto& name = scene.actors.get<Skin>(npc).animation.getStateName();
    return name == "walk-front" || name == "idle-front";
}

bool AiEngine::hasNpcReachedWaypoint(
    const sf::Vector2f position, const sf::Vector2f target)
{
    return (position - target).length() <= 1.f;
}
