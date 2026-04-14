#pragma once

#include "game/definitions/GameScene.hpp"
#include "game/definitions/NpcBlackboard.hpp"
#include "game/enums/NpcKind.hpp"
#include <DGM/classes/Time.hpp>
#include <fsm/Fsm.hpp>
#include <fsm/logging/CsvLogger.hpp>
#include <map>

class [[nodiscard]] AiEngine final
{
public:
    AiEngine(GameScene& _scene)
        : scene(_scene)
        , navMesh(scene.levelMesh.clone())
#ifdef _DEBUG
        , logger("ai_log.csv")
#endif
    {
        fsmsByKind.emplace(
            NpcKind::Melee, std::move(buildFsmForMeleeNpc(*this)));
        fsmsByKind.emplace(
            NpcKind::Ranged, std::move(buildFsmForRangedNpc(*this)));
        fsmsByKind.emplace(
            NpcKind::Turret, std::move(buildFsmForTurretNpc(*this)));

#ifdef _DEBUG
        for (auto& [_, fsm] : fsmsByKind)
            fsm.setLogger(logger);
#endif
    }

public:
    void update(const dgm::Time& time);

private: // Actions
    void waitTillAttackFinishes(NpcBlackboard& blackboard);

    void attack(NpcBlackboard& blackboard);

    void moveTowardsWaypoint(NpcBlackboard& blackboard);

    void generateWaypoint(NpcBlackboard& blackboard);

    void chooseTarget(NpcBlackboard& blackboard) {}

    void doNothing(NpcBlackboard& blackboard) {}

private: // Predicates
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

    static fsm::Fsm<NpcBlackboard> buildFsmForMeleeNpc(AiEngine& self);

    static fsm::Fsm<NpcBlackboard> buildFsmForRangedNpc(AiEngine& self);

    static fsm::Fsm<NpcBlackboard> buildFsmForTurretNpc(AiEngine& self);

private:
    GameScene& scene;
    dgm::WorldNavMesh navMesh;
#ifdef _DEBUG
    fsm::CsvLogger logger;
#endif

    std::map<NpcKind, fsm::Fsm<NpcBlackboard>> fsmsByKind;
};
