#pragma once

#include "game/definitions/GameScene.hpp"
#include "game/definitions/NpcBlackboard.hpp"
#include <DGM/classes/Time.hpp>
#include <fsm/Fsm.hpp>
#include <fsm/logging/CsvLogger.hpp>

class [[nodiscard]] AiEngine final
{
public:
    AiEngine(GameScene& _scene)
        : scene(_scene)
        , navMesh(scene.levelMesh.clone())
#ifdef _DEBUG
        , logger("ai_log.csv")
#endif
        , fsm(buildNpcFsm(*this))
    {
#ifdef _DEBUG
        fsm.setLogger(logger);
#endif
    }

public:
    void update(const dgm::Time& time);

private:
    void waitTillAttackFinishes(NpcBlackboard& blackboard);

    void attack(NpcBlackboard& blackboard);

    void moveTowardsWaypoint(NpcBlackboard& blackboard);

    void generateWaypoint(NpcBlackboard& blackboard);

private:
    bool isNpcWalking(const NpcBlackboard& blackboard) const;

    bool isCloseToTargetEntity(const NpcBlackboard& blackboard) const;

    bool hasNpcReachedWaypoint(const NpcBlackboard& blackboard) const;

    static fsm::Fsm<NpcBlackboard> buildNpcFsm(AiEngine& self);

private:
    GameScene& scene;
    dgm::WorldNavMesh navMesh;
#ifdef _DEBUG
    fsm::CsvLogger logger;
#endif
    fsm::Fsm<NpcBlackboard> fsm;
};
