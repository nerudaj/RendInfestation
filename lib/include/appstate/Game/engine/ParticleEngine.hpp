#pragma once

#include "appstate/Game/definitions/GameScene.hpp"

class [[nodiscard]] ParticleEngine
{
public:
    ParticleEngine(GameScene& scene) noexcept : scene(scene) {}

public:
    void update(dgm::Time time);

public:
    static Particle createParticle(const ParticleEmitter& emitter);

    static void updateSystem(const dgm::Time& time, ParticleSystem& system);

private:
    GameScene& scene;
};
