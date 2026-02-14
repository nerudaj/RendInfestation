#pragma once

#include "game/definitions/Actor.hpp"
#include <DGM/dgm.hpp>

struct [[nodiscard]] GameScene final
{
    dgm::DynamicBuffer<Actor> actors;
    dgm::Mesh levelMesh;
    sf::Vector2f cameraPosition = { 0.f, 0.f };
};
