#pragma once

#include "game/Types.hpp"
#include "game/definitions/GameEvents.hpp"
#include "game/definitions/GameScene.hpp"
#include "misc/EventQueue.hpp"
#include <entt/entt.hpp>

class [[nodiscard]] PhysicsEngine final
{
public:
    PhysicsEngine(GameScene& scene, EventQueue<GameEvent>& queue)
        : scene(scene), eventQueue(queue), spatialIndex(scene.levelBounds, 128)
    {
    }

public:
    void operator()(const auto&) {}

public:
    void update(const dgm::Time& time);

    template<class T>
        requires std::same_as<T, dgm::Circle> || std::same_as<T, dgm::Rect>
    void updateForConcreteCollider(
        const dgm::Time& time,
        entt::entity entity,
        T& collider,
        const ColliderOptions& options,
        PhysicsBody& body);

    template<class T>
        requires std::same_as<T, dgm::Circle> || std::same_as<T, dgm::Rect>
    void performEntityCollisionDetection(
        entt::entity entity, T& collider, sf::Vector2f& moment);

private:
    GameScene& scene;
    EventQueue<GameEvent>& eventQueue;
    dgm::SpatialIndex<entt::entity> spatialIndex;
};
