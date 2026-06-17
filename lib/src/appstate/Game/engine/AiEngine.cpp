#include "appstate/Game/engine/AiEngine.hpp"
#include "misc/Compatibility.hpp"
#include <DGM/classes/Raycaster.hpp>
#include <fsm/Builder.hpp>

void AiEngine::update(const dgm::Time&)
{
    // loop over all npcs (they have NpcInputs)
    // throw their logic into some FSM
    // update input

    for (auto&& [_, blackboard] : scene.actors.view<NpcBlackboard>().each())
    {
        if (blackboard.targetEntity == entt::null
            && blackboard.kind != NpcKind::Turret)
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
    assert(blackboard.waypoint);
    const auto position =
        scene.actors.get<Collider>(blackboard.ownerEntity).getPosition();
    const auto direction = dgm::Math::toUnit(*blackboard.waypoint - position);
    moveInDirection(blackboard, direction);
}

void AiEngine::moveTowardsTarget(NpcBlackboard& blackboard)
{
    const auto direction = getDirectionToTarget(blackboard);
    moveInDirection(blackboard, direction);
}

void AiEngine::generateWaypointInFrontOfTarget(NpcBlackboard& blackboard)
{
    const auto targetPosition =
        scene.actors.get<Collider>(blackboard.targetEntity).getPosition();
    const auto targetDirection =
        scene.actors.get<LookDirection>(blackboard.targetEntity).get();

    const auto raycasterResult = dgm::Raycaster::raycast(
        targetPosition, targetDirection, scene.levelMesh);
    const auto directionToHit = raycasterResult.hitLocation - targetPosition;

    // NOTE: multiplying by 0.9f so the target location doesn't lie in a solid
    // wall
    computePathAndUpdateWaypoint(
        targetPosition + directionToHit * 0.9f, navMesh, blackboard);
}

void AiEngine::generateWaypointBehindOfTarget(NpcBlackboard& blackboard)
{
    const auto targetPosition =
        scene.actors.get<Collider>(blackboard.targetEntity).getPosition();
    const auto targetDirection =
        scene.actors.get<LookDirection>(blackboard.targetEntity).get();

    // NOTE: Inverted targetDirection
    const auto raycasterResult = dgm::Raycaster::raycast(
        targetPosition, -targetDirection, scene.levelMesh);
    const auto directionToHit = raycasterResult.hitLocation - targetPosition;

    // NOTE: multiplying by 0.9f so the target location doesn't lie in a solid
    // wall
    computePathAndUpdateWaypoint(
        targetPosition + directionToHit * 0.9f, navMesh, blackboard);
}

void AiEngine::generateWaypointShortestPath(NpcBlackboard& blackboard)
{
    computePathAndUpdateWaypoint(
        scene.actors.get<Collider>(blackboard.targetEntity).getPosition(),
        navMesh,
        blackboard);
}

void AiEngine::generateWaypointForFlyingNpc(NpcBlackboard& blackboard)
{
    computePathAndUpdateWaypoint(
        scene.actors.get<Collider>(blackboard.targetEntity).getPosition(),
        altNavMesh,
        blackboard);
}

void AiEngine::lookAtTarget(NpcBlackboard& blackboard)
{
    waitTillAttackFinishes(blackboard);
}

void AiEngine::choosePlayerAsTarget(NpcBlackboard& blackboard)
{
    blackboard.targetEntity = scene.playerEntity;
}

void AiEngine::chooseTargetForEnemyNpc(NpcBlackboard& blackboard)
{
    blackboard.targetEntity = chooseTarget(
        EntityKind::Player,
        scene.actors.get<Collider>(blackboard.ownerEntity).getPosition());
}

void AiEngine::chooseTargetForFriendlyNpc(NpcBlackboard& blackboard)
{
    blackboard.targetEntity = chooseTarget(
        EntityKind::Npc,
        scene.actors.get<Collider>(blackboard.ownerEntity).getPosition());
}

void AiEngine::invalidateWaypoint(NpcBlackboard& blackboard)
{
    blackboard.waypoint.reset();
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

bool AiEngine::isTargetVisibleOnAltMesh(const NpcBlackboard& blackboard) const
{
    return dgm::Raycaster::hasDirectVisibility(
        scene.actors.get<Collider>(blackboard.ownerEntity).getPosition(),
        scene.actors.get<Collider>(blackboard.targetEntity).getPosition(),
        scene.altLevelMesh);
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
    if (!blackboard.waypoint) return true;

    if (blackboard.waypoint == sf::Vector2f {}) return true;
    const auto position =
        scene.actors.get<Collider>(blackboard.ownerEntity).getPosition();
    return (position - *blackboard.waypoint).length() <= 1.f;
}

bool AiEngine::isTargetVisibleForShooting(const NpcBlackboard& blackboard) const
{
    return dgm::Raycaster::hasDirectVisibility(
        scene.actors.get<Collider>(blackboard.ownerEntity).getPosition(),
        scene.actors.get<Collider>(blackboard.targetEntity).getPosition(),
        scene.altLevelMesh);
}

bool AiEngine::isGunReady(const NpcBlackboard& blackboard) const
{
    auto&& weaponInventory =
        scene.actors.try_get<WeaponInventory>(blackboard.ownerEntity);
    if (!weaponInventory) return false;

    return weaponInventory->weapons[weaponInventory->activeWeapon].timeTillFire
           <= sf::Time::Zero;
}

bool AiEngine::isTargetValidAndVisibleAndInShootingRange(
    const NpcBlackboard& blackboard) const
{
    return hasValidTarget(blackboard) && isTargetVisibleForShooting(blackboard)
           && isTargetInShootingRange(blackboard);
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
        scene.actors.get<Collider>(blackboard.targetEntity).getPosition();
    return targetPosition - position;
}

void AiEngine::moveInDirection(
    NpcBlackboard& blackboard, const sf::Vector2f& direction)
{
    const auto unitDirection = dgm::Math::toUnit(direction);
    blackboard.input.setForward(unitDirection);
    blackboard.input.setAimDirection(unitDirection);
}

void AiEngine::computePathAndUpdateWaypoint(
    const sf::Vector2f& targetPosition,
    dgm::WorldNavMesh& navigationMesh,
    NpcBlackboard& blackboard)
{
    const auto position =
        scene.actors.get<Collider>(blackboard.ownerEntity).getPosition();
    auto path = navigationMesh.computePath(position, targetPosition);
    if (!path.isTraversed()) blackboard.waypoint = path.getCurrentPoint().coord;
}

entt::entity AiEngine::chooseTarget(
    EntityKind eligibleEntityKind, const sf::Vector2f& thisEntityPosition)
{
    using EntityPositionPair = std::pair<entt::entity, sf::Vector2f>;

    auto&& targets = std::vector<EntityPositionPair>();

    for (auto&& [entity, collider, skin] :
         scene.actors.view<Collider, Skin>().each())
    {
        if (skin.kind != eligibleEntityKind) continue;

        targets.push_back({ entity, collider.getPosition() });
    }

    uni::ranges::sort(
        targets,
        [&](const EntityPositionPair& a, const EntityPositionPair& b) -> bool
        {
            const auto aDist = (a.second - thisEntityPosition).lengthSquared();
            const auto bDist = (b.second - thisEntityPosition).lengthSquared();
            return aDist < bDist;
        });

    assert(!targets.empty());
    return targets.front().first;
}

#define CONDITION(x) [&](const NpcBlackboard& b) -> bool { return self.x(b); }

#define NOT(x) [&](const NpcBlackboard& b) -> bool { return !self.x(b); }

#define ACTION(x) [&](NpcBlackboard& b) { self.x(b); }

fsm::Fsm<NpcBlackboard> AiEngine::buildFsmForScuttlebug(AiEngine& self)
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
                .exec(ACTION(generateWaypointInFrontOfTarget))
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

fsm::Fsm<NpcBlackboard> AiEngine::buildFsmForGreaterScuttlebug(AiEngine& self)
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
                .exec(ACTION(generateWaypointBehindOfTarget))
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

fsm::Fsm<NpcBlackboard> AiEngine::buildFsmForBighead(AiEngine& self)
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
                .exec(ACTION(generateWaypointShortestPath))
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

fsm::Fsm<NpcBlackboard> AiEngine::buildFsmForBeholder(AiEngine& self)
{
    // clang-format off
    return fsm::Builder<NpcBlackboard>()
        .withErrorMachine()
            .useGlobalEntryCondition(NOT(hasValidTarget))
                .withEntryState("Start")
                    .exec(ACTION(choosePlayerAsTarget))
                    .andRestart()
            .done()
        .withMainMachine()
            .withEntryState("Start")
                .when(CONDITION(isTargetValidAndVisibleAndInShootingRange))
                    .goToState("Attack")
                .orWhen(CONDITION(isTargetVisibleOnAltMesh))
                    .goToState("InvalidateWaypoint")
                .orWhen(CONDITION(hasNpcReachedWaypoint))
                    .goToState("GenerateWaypoint")
                .otherwiseExec(ACTION(moveTowardsWaypoint))
                .andLoop()
            .withState("InvalidateWaypoint")
                .exec(ACTION(invalidateWaypoint))
                .andGoToState("MoveTowardsTarget")
            .withState("MoveTowardsTarget")
                .when(CONDITION(isTargetInShootingRange))
                    .goToState("Attack")
                .orWhen(NOT(isTargetVisible))
                    .goToState("Start")
                .otherwiseExec(ACTION(moveTowardsTarget))
                .andLoop()
            .withState("GenerateWaypoint")
                .exec(ACTION(generateWaypointForFlyingNpc))
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
        .withErrorMachine()
            .useGlobalEntryCondition(NOT(hasValidTarget))
                .withEntryState("Sink")
                    .exec(ACTION(chooseTargetForFriendlyNpc))
                    .andRestart()
            .done()
        .withMainMachine()
            .withEntryState("Start")
                .exec(ACTION(chooseTargetForFriendlyNpc))
                .andGoToState("WaitingForSpawnToFinish")
            .withState("WaitingForSpawnToFinish")
                .when(CONDITION(isOwnerIdleOrWalking))
                    .goToState("Idle")
                .otherwiseExec(ACTION(doNothing))
                .andLoop()
            .withState("Idle")
                .when(NOT(isTargetValidAndVisibleAndInShootingRange))
                    .goToState("Start")
                .otherwiseExec(ACTION(lookAtTarget))
                .andGoToState("WaitForGunReady")
            .withState("WaitForGunReady")
                .when(NOT(isTargetValidAndVisibleAndInShootingRange))
                    .goToState("Start")
                .orWhen(CONDITION(isGunReady))
                    .goToState("Attack")
                .otherwiseExec(ACTION(lookAtTarget))
                .andLoop()
            .withState("Attack")
            .exec(ACTION(attack))
            .andGoToState("WaitUntilAttackFinishes")
        .withState("WaitUntilAttackFinishes")
            .when(CONDITION(isOwnerIdleOrWalking))
                .goToState("Idle")
            .otherwiseExec(ACTION(lookAtTarget))
            .andLoop()
        .done()
    .build();
    // clang-format on
}

#undef CONDITION
#undef NOT
#undef ACTION
