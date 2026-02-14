#include "game/engine/GameRulesEngine.hpp"
#include <algorithm>
#include <limits>

const float SPEED = 192.f;

void GameRulesEngine::update(const dgm::Time& time)
{
    for (auto&& [actor, _] : scene.actors)
    {
        if (actor.kind != ActorKind::Player) continue;

        auto& body = std::get<dgm::Circle>(actor.body);

        actor.forward = input.getForward() * SPEED;
        actor.forward.x = std::clamp(actor.forward.x, -SPEED, SPEED);
        actor.forward.y = std::clamp(actor.forward.y, -SPEED, SPEED);

        auto moment = actor.forward * time.getDeltaTime();
        dgm::Collision::advanced(scene.levelMesh, body, moment);
        body.move(moment);
        scene.cameraPosition = body.getPosition();
    }
}
