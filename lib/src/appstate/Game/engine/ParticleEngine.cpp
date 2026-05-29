#include "appstate/Game/engine/ParticleEngine.hpp"

static float randomFloat(float min, float max, float scale = 100.f)
{
    return (rand() % static_cast<int>(max - min) * scale) / scale + min;
}

void ParticleEngine::update(dgm::Time time)
{
    for (auto&& [entity, emitter, system] :
         scene.actors.view<ParticleEmitter, ParticleSystem>().each())
    {
        emitter.emissionTimer -= time.getElapsed();
        if (emitter.emissionTimer <= sf::Time::Zero
            && emitter.particlesToEmit > 0)
        {
            emitter.emissionTimer = emitter.emissionInterval;
            --emitter.particlesToEmit;

            system.particles.emplace_back(createParticle(emitter));
        }

        updateSystem(time, system);
    }
}

Particle ParticleEngine::createParticle(const ParticleEmitter& emitter)
{
    const auto angle = sf::degrees(randomFloat(
        -emitter.directionVariance.asDegrees(),
        emitter.directionVariance.asDegrees()));
    const auto speed = randomFloat(
        emitter.speed - emitter.speedVariance,
        emitter.speed + emitter.speedVariance);

    return Particle {
        .position = emitter.position,
        .velocity = emitter.direction.rotatedBy(angle) * speed,
        .size = sf::Vector2f { 1.f, 1.f }
                * (emitter.maxProjectileSize == 1.f
                       ? 1.f
                       : randomFloat(1.f, emitter.maxProjectileSize, 1.f)),
        .color = emitter.colors[rand() % 2],
    };
}

void ParticleEngine::updateSystem(const dgm::Time& time, ParticleSystem& system)
{
    for (auto&& particle : system.particles)
    {
        particle.position += particle.velocity * time.getDeltaTime();
    }
}
