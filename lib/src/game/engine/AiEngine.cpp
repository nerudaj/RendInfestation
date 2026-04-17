#include "game/engine/AiEngine.hpp"
#include <DGM/classes/Raycaster.hpp>
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
        fsmsByKind.at(std::to_underlying(blackboard.kind)).tick(blackboard);
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
    moveInDirection(blackboard, direction);
}

void AiEngine::moveTowardsTarget(NpcBlackboard& blackboard)
{
    const auto direction = getDirectionToTarget(blackboard);
    moveInDirection(blackboard, direction);
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

// ==========
// PREDICATES
// ==========

bool AiEngine::isTargetVisible(const NpcBlackboard& blackboard) const
{
    return dgm::Raycaster::hasDirectVisibility(
        scene.actors.get<Collider>(blackboard.ownerEntity).getPosition(),
        scene.actors.get<Collider>(blackboard.targetEntity).getPosition(),
        scene.levelMesh);
}

bool AiEngine::isTargetInMeleeRange(const NpcBlackboard& blackboard) const
{
    return getDirectionToTarget(blackboard).length() <= 20.f;
}

bool AiEngine::isTargetInShootingRange(const NpcBlackboard& blackboard) const
{
    return getDirectionToTarget(blackboard).length() <= 160.f;
}

bool AiEngine::isOwnerIdleOrWalking(const NpcBlackboard& blackboard) const
{
    const auto& name =
        scene.actors.get<Skin>(blackboard.ownerEntity).animation.getStateName();
    return name == WALK_ANIMATION_STATE || name == IDLE_ANIMATION_STATE;
}

bool AiEngine::hasNpcReachedWaypoint(const NpcBlackboard& blackboard) const
{
    if (blackboard.waypoint == sf::Vector2f {}) return true;
    const auto position =
        scene.actors.get<Collider>(blackboard.ownerEntity).getPosition();
    return (position - blackboard.waypoint).length() <= 1.f;
}

// =====
// UTILS
// =====
sf::Vector2f
AiEngine::getDirectionToTarget(const NpcBlackboard& blackboard) const
{
    const auto position =
        scene.actors.get<Collider>(blackboard.ownerEntity).getPosition();
    const auto targetPosition =
        scene.actors.get<Collider>(scene.playerEntity).getPosition();
    return targetPosition - position;
}

void AiEngine::moveInDirection(
    NpcBlackboard& blackboard, const sf::Vector2f& direction)
{
    const auto unitDirection = dgm::Math::toUnit(direction);
    blackboard.input.setForward(unitDirection);
    blackboard.input.setAimDirection(unitDirection);
}

#define CONDITION(x) [&](const NpcBlackboard& b) -> bool { return self.x(b); }

#define NOT(x) [&](const NpcBlackboard& b) -> bool { return !self.x(b); }

#define ACTION(x) [&](NpcBlackboard& b) { self.x(b); }

fsm::Fsm<NpcBlackboard> AiEngine::buildFsmForMeleeNpc(AiEngine& self)
{
    // clang-format off
    return fsm::Builder<NpcBlackboard>()
        .withNoErrorMachine()
        .withMainMachine()
            .withEntryState("Start")
                .when(CONDITION(isTargetInMeleeRange))
                    .goToState("Attack")
                .orWhen(CONDITION(isTargetVisible))
                    .goToState("MoveTowardsTarget")
                .orWhen(CONDITION(hasNpcReachedWaypoint))
                    .goToState("GenerateWaypoint")
                .otherwiseExec(ACTION(moveTowardsWaypoint))
                .andLoop()
            .withState("MoveTowardsTarget")
                .when(CONDITION(isTargetInMeleeRange))
                    .goToState("Attack")
                .orWhen(NOT(isTargetVisible))
                    .goToState("Start")
                .otherwiseExec(ACTION(moveTowardsTarget))
                .andLoop()
            .withState("GenerateWaypoint")
                .exec(ACTION(generateWaypoint))
                .andGoToState("Start")
            .withState("Attack")
                .exec(ACTION(attack))
                .andGoToState("WaitUntilAttackFinishes")
            .withState("WaitUntilAttackFinishes")
                .when(CONDITION(isOwnerIdleOrWalking))
                    .goToState("Start")
                .otherwiseExec(ACTION(waitTillAttackFinishes))
                .andLoop()
        .done()
    .build();
    // clang-format on
}

fsm::Fsm<NpcBlackboard> AiEngine::buildFsmForRangedNpc(AiEngine& self)
{
    // clang-format off
    return fsm::Builder<NpcBlackboard>()
        .withNoErrorMachine()
        .withMainMachine()
            .withEntryState("Start")
                .when(CONDITION(isTargetInShootingRange))
                    .goToState("Attack")
                .orWhen(CONDITION(isTargetVisible))
                    .goToState("MoveTowardsTarget")
                .orWhen(CONDITION(hasNpcReachedWaypoint))
                    .goToState("GenerateWaypoint")
                .otherwiseExec(ACTION(moveTowardsWaypoint))
                .andLoop()
            .withState("MoveTowardsTarget")
                .when(CONDITION(isTargetInMeleeRange))
                    .goToState("Attack")
                .orWhen(NOT(isTargetVisible))
                    .goToState("Start")
                .otherwiseExec(ACTION(moveTowardsTarget))
                .andLoop()
            .withState("GenerateWaypoint")
                .exec(ACTION(generateWaypoint))
                .andGoToState("Start")
            .withState("Attack")
                .exec(ACTION(attack))
                .andGoToState("WaitUntilAttackFinishes")
            .withState("WaitUntilAttackFinishes")
                .when(CONDITION(isOwnerIdleOrWalking))
                    .goToState("Start")
                .otherwiseExec(ACTION(waitTillAttackFinishes))
                .andLoop()
        .done()
    .build();
    // clang-format on
}

fsm::Fsm<NpcBlackboard> AiEngine::buildFsmForTurretNpc(AiEngine& self)
{
    // clang-format off
    return fsm::Builder<NpcBlackboard>()
        .withNoErrorMachine()
        .withMainMachine()
            .withEntryState("Start")
                .when(NOT(hasValidTarget))
                    .goToState("PickTarget")
                .orWhen(CONDITION(isTargetInShootingRange))
                    .goToState("Attack")
                .otherwiseExec(ACTION(doNothing))
                .andLoop()
            .withState("PickTarget")
                .exec(ACTION(chooseTarget))
                .andGoToState("Start")
            .withState("Attack")
                .exec(ACTION(attack))
                .andGoToState("WaitUntilAttackFinishes")
            .withState("WaitUntilAttackFinishes")
                .when(CONDITION(isOwnerIdleOrWalking))
                    .goToState("Start")
                .otherwiseExec(ACTION(doNothing))
                .andLoop()
        .done()
    .build();
    // clang-format on
}

#undef CONDITION
#undef NOT
#undef ACTION
