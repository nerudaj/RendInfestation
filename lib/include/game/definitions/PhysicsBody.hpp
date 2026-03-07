#pragma once

#include "types/Overloads.hpp"
#include <DGM/classes/Collision.hpp>
#include <DGM/classes/Objects.hpp>
#include <variant>

struct [[nodiscard]] ColliderOptions final
{
    bool reportMeshCollisions = false;
    bool reportActorCollisions = false;
    bool nonblocking = false;
    bool disabled = false;
};

struct [[nodiscard]] Collider final
{
    std::variant<dgm::Rect, dgm::Circle> shape;
    ColliderOptions options;

    template<class T>
        requires std::same_as<T, dgm::Rect> || std::same_as<T, dgm::Circle>
    bool collidesWith(const T& otherShape) const
    {
        return std::visit(
            overloads {
                [&](const dgm::Rect& r)
                { return dgm::Collision::basic(r, otherShape); },
                [&](const dgm::Circle& c)
                { return dgm::Collision::basic(otherShape, c); },
            },
            shape);
    }

    sf::Vector2f getPosition() const
    {
        return std::visit(
            overloads {
                [](const dgm::Rect& r) { return r.getCenter(); },
                [](const dgm::Circle& c) { return c.getPosition(); },
            },
            shape);
    }

    float getRadius() const
    {
        return std::visit(
            overloads {
                [](const dgm::Rect& r) { return r.getSize().length(); },
                [](const dgm::Circle& c) { return c.getRadius(); },
            },
            shape);
    }

    void move(const sf::Vector2f& vec)
    {
        std::visit(
            overloads {
                [&vec](auto& b) { b.move(vec); },
            },
            shape);
    }
};

struct [[nodiscard]] PhysicsBody final
{
    sf::Vector2f forward = { 0.f, 0.f };
    float bounciness = 0.f;
    float friction = 0.5f;
    bool useAltMesh = false;
};
