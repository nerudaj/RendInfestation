#include "game/engine/AiEngine.hpp"
#include <fsm/Builder.hpp>

void AiEngine::update(const dgm::Time& time)
{
    // loop over all npcs (they have NpcInputs)
    // throw their logic into some FSM
    // update input

    for (auto&& [_, blackboard] : scene.actors.view<NpcBlackboard>().each())
    {
        if (blackboard.targetEntity == entt::null)
            blackboard.targetEntity = scene.playerEntity;
        blackboard.input.clearInputs();

        fsm.tick(blackboard);
    }
}

void AiEngine::waitTillAttackFinishes(NpcBlackboard& blackboard)
{
    blackboard.input.setAimDirection(dgm::Math::toUnit(
        scene.actors.get<Collider>(blackboard.targetEntity).getPosition()
        - scene.actors.get<Collider>(blackboard.ownerEntity).getPosition()));
}

void AiEngine::attack(NpcBlackboard& blackboard)
{
    blackboard.input.setShooting(true);
}

void AiEngine::moveTowardsWaypoint(NpcBlackboard& blackboard)
{
    const auto position =
        scene.actors.get<Collider>(blackboard.ownerEntity).getPosition();
    const auto direction = dgm::Math::toUnit(blackboard.waypoint - position);

    blackboard.input.setForward(direction);
    blackboard.input.setAimDirection(direction);
}

void AiEngine::generateWaypoint(NpcBlackboard& blackboard)
{
    const auto position =
        scene.actors.get<Collider>(blackboard.ownerEntity).getPosition();
    const auto targetPosition =
        scene.actors.get<Collider>(scene.playerEntity).getPosition();
    auto path = navMesh.computePath(position, targetPosition);

    if (!path.isTraversed()) blackboard.waypoint = path.getCurrentPoint().coord;
}

bool AiEngine::isNpcWalking(const NpcBlackboard& blackboard) const
{
    const auto& name =
        scene.actors.get<Skin>(blackboard.ownerEntity).animation.getStateName();
    return name == "walk-front" || name == "idle-front";
}

bool AiEngine::isCloseToTargetEntity(const NpcBlackboard& blackboard) const
{
    const auto position =
        scene.actors.get<Collider>(blackboard.ownerEntity).getPosition();
    const auto targetPosition =
        scene.actors.get<Collider>(scene.playerEntity).getPosition();
    return (position - targetPosition).length() <= 20.f;
}

bool AiEngine::hasNpcReachedWaypoint(const NpcBlackboard& blackboard) const
{
    if (blackboard.waypoint == sf::Vector2f {}) return true;
    const auto position =
        scene.actors.get<Collider>(blackboard.ownerEntity).getPosition();
    return (position - blackboard.waypoint).length() <= 1.f;
}

#define CONDITION(x) [&](const NpcBlackboard& b) -> bool { return self.x(b); }

#define NOT(x) [&](const NpcBlackboard& b) -> bool { return self.x(b); }

#define ACTION(x) [&](NpcBlackboard& b) { self.x(b); }

fsm::Fsm<NpcBlackboard> AiEngine::buildNpcFsm(AiEngine& self)
{
    // clang-format off
    return fsm::Builder<NpcBlackboard>()
        .withNoErrorMachine()
        .withMainMachine()
            .withEntryState("Start")
                .when(CONDITION(isCloseToTargetEntity))
                    .goToState("Attack")
                .orWhen(CONDITION(hasNpcReachedWaypoint))
                    .goToState("GenerateWaypoint")
                .otherwiseExec(ACTION(moveTowardsWaypoint))
                .andLoop()
            .withState("GenerateWaypoint")
                .exec(ACTION(generateWaypoint))
                .andGoToState("Start")
            .withState("Attack")
                .exec(ACTION(attack))
                .andGoToState("WaitUntilAttackFinishes")
            .withState("WaitUntilAttackFinishes")
                .when(CONDITION(isNpcWalking))
                    .goToState("Start")
                .otherwiseExec(ACTION(waitTillAttackFinishes))
                .andLoop()
        .done()
    .build();
    // clang-format on
}

#undef CONDITION;
#undef ACTION;
