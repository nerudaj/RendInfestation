#pragma once

#include <DGM/classes/Objects.hpp>

struct [[nodiscard]] DamageMarker final
{
    dgm::Circle hitbox;
    int damage = 0;
    bool playerOrigin = false;
};
