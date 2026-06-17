#pragma once

#include "appstate/Game/definitions/GameScene.hpp"
#include "appstate/Game/definitions/NpcBlackboard.hpp"
#include "appstate/Game/enums/NpcKind.hpp"
#include <DGM/classes/Time.hpp>
#include <array>
#include <fsm/Fsm.hpp>
#include <fsm/logging/CsvLogger.hpp>
#include <map>

#ifdef _DEBUG
#define FSM_LOGGING_ENABLED 1
#endif

class [[nodiscard]] AiEngine final
{
public:
    AiEngine(GameScene& _scene)
        : scene(_scene)
        , navMesh(scene.levelMesh.clone())
        , altNavMesh(scene.altLevelMesh.clone())
#ifdef FSM_LOGGING_ENABLED
        , logger("ai_log.csv")
#endif
        , fsmsByKind(std::array { buildFsmForScuttlebug(*this),
                                  buildFsmForGreaterScuttlebug(*this),
                                  buildFsmForBighead(*this),
                                  buildFsmForBeholder(*this),
                                  buildFsmForTurretNpc(*this) })
    {
#ifdef FSM_LOGGING_ENABLED
        for (auto& fsm : fsmsByKind)
            fsm.setLogger(logger);
#endif
    }

public:
    void update(const dgm::Time& time);

private: // Actions
    void waitTillAttackFinishes(NpcBlackboard& blackboard);

    void attack(NpcBlackboard& blackboard);

    void moveTowardsWaypoint(NpcBlackboard& blackboard);

    void moveTowardsTarget(NpcBlackboard& blackboard);

    void generateWaypointInFrontOfTarget(NpcBlackboard& blackboard);

    void generateWaypointBehindOfTarget(NpcBlackboard& blackboard);

    void generateWaypointShortestPath(NpcBlackboard& blackboard);

    void generateWaypointForFlyingNpc(NpcBlackboard& blackboard);

    void lookAtTarget(NpcBlackboard& blackboard);

    void choosePlayerAsTarget(NpcBlackboard& blackboard);

    void chooseTargetForEnemyNpc(NpcBlackboard& blackboard);

    void chooseTargetForFriendlyNpc(NpcBlackboard& blackboard);

    void invalidateWaypoint(NpcBlackboard& blackboard);

    void doNothing(NpcBlackboard&) {}

private: // Predicates
    bool isTargetVisible(const NpcBlackboard& blackboard) const;

    bool isTargetVisibleOnAltMesh(const NpcBlackboard& blackboard) const;

    bool isTargetInMeleeRange(const NpcBlackboard& blackboard) const;

    bool isTargetInShootingRange(const NpcBlackboard& blackboard) const;

    bool isOwnerIdleOrWalking(const NpcBlackboard& blackboard) const;

    bool hasNpcReachedWaypoint(const NpcBlackboard& blackboard) const;

    bool hasValidTarget(const NpcBlackboard& blackboard) const
    {
        return blackboard.targetEntity != entt::null
               && scene.actors.valid(blackboard.targetEntity);
    }

    bool isTargetVisibleForShooting(const NpcBlackboard& blackboard) const;

    bool isGunReady(const NpcBlackboard& blackboard) const;

    bool isTargetValidAndVisibleAndInShootingRange(
        const NpcBlackboard& blackboard) const;

private: // Utils
    sf::Vector2f getDirectionToTarget(const NpcBlackboard& blackboard) const;

    void
    moveInDirection(NpcBlackboard& blackboard, const sf::Vector2f& direction);

    void computePathAndUpdateWaypoint(
        const sf::Vector2f& targetPosition,
        dgm::WorldNavMesh& navigationMesh,
        NpcBlackboard& blackboard);

    std::optional<entt::entity> chooseTarget(
        EntityKind eligibleEntityKind, const sf::Vector2f& thisEntityPosition);

    static fsm::Fsm<NpcBlackboard> buildFsmForScuttlebug(AiEngine& self);

    static fsm::Fsm<NpcBlackboard> buildFsmForGreaterScuttlebug(AiEngine& self);

    static fsm::Fsm<NpcBlackboard> buildFsmForBighead(AiEngine& self);

    static fsm::Fsm<NpcBlackboard> buildFsmForBeholder(AiEngine& self);

    static fsm::Fsm<NpcBlackboard> buildFsmForTurretNpc(AiEngine& self);

private:
    GameScene& scene;
    dgm::WorldNavMesh navMesh;
    dgm::WorldNavMesh altNavMesh;
#ifdef FSM_LOGGING_ENABLED
    fsm::CsvLogger logger;
#endif

    std::array<fsm::Fsm<NpcBlackboard>, static_cast<size_t>(NpcKind::Max)>
        fsmsByKind;
};
