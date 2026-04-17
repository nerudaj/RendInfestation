#pragma once

#include "game/definitions/GameScene.hpp"
#include "game/definitions/NpcBlackboard.hpp"
#include "game/enums/NpcKind.hpp"
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
#ifdef FSM_LOGGING_ENABLED
        , logger("ai_log.csv")
#endif
        , fsmsByKind(std::array { buildFsmForMeleeNpc(*this),
                                  buildFsmForRangedNpc(*this),
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

    void generateWaypoint(NpcBlackboard& blackboard);

    void chooseTarget(NpcBlackboard&) {}

    void doNothing(NpcBlackboard&) {}

private: // Predicates
    bool isTargetVisible(const NpcBlackboard& blackboard) const;

    bool isTargetInMeleeRange(const NpcBlackboard& blackboard) const;

    bool isTargetInShootingRange(const NpcBlackboard& blackboard) const;

    bool isOwnerIdleOrWalking(const NpcBlackboard& blackboard) const;

    bool hasNpcReachedWaypoint(const NpcBlackboard& blackboard) const;

    bool hasValidTarget(const NpcBlackboard& blackboard) const
    {
        return blackboard.targetEntity != entt::null;
    }

private: // Utils
    sf::Vector2f getDirectionToTarget(const NpcBlackboard& blackboard) const;

    void
    moveInDirection(NpcBlackboard& blackboard, const sf::Vector2f& direction);

    static fsm::Fsm<NpcBlackboard> buildFsmForMeleeNpc(AiEngine& self);

    static fsm::Fsm<NpcBlackboard> buildFsmForRangedNpc(AiEngine& self);

    static fsm::Fsm<NpcBlackboard> buildFsmForTurretNpc(AiEngine& self);

private:
    GameScene& scene;
    dgm::WorldNavMesh navMesh;
#ifdef FSM_LOGGING_ENABLED
    fsm::CsvLogger logger;
#endif

    std::array<fsm::Fsm<NpcBlackboard>, static_cast<size_t>(NpcKind::Max)>
        fsmsByKind;
};
