#pragma once

#include "appstate/Game/definitions/Constants.hpp"
#include "appstate/Game/enums/ParticleSystemKind.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <map>
#include <vector>

struct [[nodiscard]] ParticleSystemDefinition final
{
    sf::Time emissionInterval;
    sf::Time lifetime;
    int particlesToEmit;
    sf::Angle directionVariance;
    std::vector<sf::Color> colors;
    float maxParticleSize;
    float speed;
    float speedVariance;
};

const std::map<ParticleSystemKind, ParticleSystemDefinition>
    PARTICLE_SYSTEM_DEF = {
        {
            ParticleSystemKind::BloodSpatter,
            ParticleSystemDefinition {
                .emissionInterval = sf::Time::Zero,
                .lifetime = sf::seconds(0.5f),
                .particlesToEmit = 16,
                .directionVariance = sf::degrees(100),
                .colors = { COLOR_RED, COLOR_DARK_RED },
                .maxParticleSize = 4.f,
                .speed = 30.f,
                .speedVariance = 10.f,
            },
        },
        {
            ParticleSystemKind::ProjectileImpact,
            ParticleSystemDefinition {
                .emissionInterval = sf::Time::Zero,
                .lifetime = sf::seconds(0.25f),
                .particlesToEmit = 10,
                .directionVariance = sf::degrees(15),
                .colors = { COLOR_ORANGE, COLOR_YELLOW },
                .maxParticleSize = 1.f,
                .speed = 60.f,
                .speedVariance = 10.f,
            },
        },
        {
            ParticleSystemKind::CactusSpatter,
            ParticleSystemDefinition {
                .emissionInterval = sf::Time::Zero,
                .lifetime = sf::seconds(0.5f),
                .particlesToEmit = 16,
                .directionVariance = sf::degrees(100),
                .colors = { COLOR_GREEN, COLOR_DARK_GREEN },
                .maxParticleSize = 4.f,
                .speed = 30.f,
                .speedVariance = 10.f,
            },
        },
        {
            ParticleSystemKind::SmallExplosion,
            ParticleSystemDefinition {
                .emissionInterval = sf::Time::Zero,
                .lifetime = sf::seconds(0.2f),
                .particlesToEmit = 24,
                .directionVariance = sf::degrees(360),
                .colors = { COLOR_DARK_GREY,
                            COLOR_ORANGE,
                            COLOR_YELLOW,
                            COLOR_WHITE },
                .maxParticleSize = 4.f,
                .speed = 60.f,
                .speedVariance = 30.f,
            },
        },
        {
            ParticleSystemKind::Explosion,
            ParticleSystemDefinition {
                .emissionInterval = sf::Time::Zero,
                .lifetime = sf::seconds(0.35f),
                .particlesToEmit = 48,
                .directionVariance = sf::degrees(360),
                .colors = { COLOR_DARK_GREY,
                            COLOR_ORANGE,
                            COLOR_YELLOW,
                            COLOR_WHITE },
                .maxParticleSize = 7.f,
                .speed = 60.f,
                .speedVariance = 30.f,
            },
        },
        {
            ParticleSystemKind::HugeExplosion,
            ParticleSystemDefinition {
                .emissionInterval = sf::Time::Zero,
                .lifetime = sf::seconds(0.5f),
                .particlesToEmit = 48,
                .directionVariance = sf::degrees(360),
                .colors = { COLOR_DARK_GREY,
                            COLOR_ORANGE,
                            COLOR_YELLOW,
                            COLOR_WHITE },
                .maxParticleSize = 10.f,
                .speed = 60.f,
                .speedVariance = 30.f,
            },
        },
        {
            ParticleSystemKind::FireballExplosion,
            ParticleSystemDefinition {
                .emissionInterval = sf::Time::Zero,
                .lifetime = sf::seconds(0.4f),
                .particlesToEmit = 24,
                .directionVariance = sf::degrees(360),
                .colors = { COLOR_RED, COLOR_ORANGE },
                .maxParticleSize = 6.f,
                .speed = 60.f,
                .speedVariance = 0.f,
            },
        },
    };
