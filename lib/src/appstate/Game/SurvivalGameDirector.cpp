#include "appstate/Game/SurvivalGameDirector.hpp"

SurvivalGameDirector::SurvivalGameDirector() : context(createInitialContext())
{
}

void SurvivalGameDirector::update(
    const sf::Time& time, EventQueue<GameEvent>& eventQueue)
{
    if (context.state == SurvivalModeState::WaitingForNextWave)
    {
        context.timeTillNextWave -= time;
        if (context.timeTillNextWave <= sf::Time::Zero)
        {
            ++context.wave;
            context.enemiesSpawnedInCurrentWave = 0;
            context.enemiesKilledInCurrentWave = 0;
            context.enemiesInCurrentWave = getEnemyCountForWave(context.wave);
            context.state = SurvivalModeState::SpawningEnemies;
        }
    }
    else if (context.state == SurvivalModeState::SpawningEnemies)
    {
        context.timeTillNextSpawn -= time;
        if (context.timeTillNextSpawn <= sf::Time::Zero)
        {
            context.timeTillNextSpawn = SPAWNER_SPAWN_DELAY;
            ++context.enemiesSpawnedInCurrentWave;
            eventQueue.pushEvent<event::SurvivalSpawnerTimerHit>(
                getEnemyToSpawn());

            if (context.enemiesInCurrentWave
                == context.enemiesSpawnedInCurrentWave)
            {
                context.state = SurvivalModeState::WaitingForEnemiesToDie;
            }
        }
    }
    else if (context.state == SurvivalModeState::WaitingForEnemiesToDie)
    {
        if (context.enemiesKilledInCurrentWave == context.enemiesInCurrentWave)
        {
            context.state = SurvivalModeState::WaitingForNextWave;
            context.timeTillNextWave = sf::seconds(5.f);
            eventQueue.pushEvent<event::WaveEnded>();
        }
    }
}

int SurvivalGameDirector::markKilledEnemy(const Skin& skin)
{
    ++context.enemiesKilledInCurrentWave;
    return getScoreForEnemy(skin);
}

int SurvivalGameDirector::getScoreForEnemy(const Skin& skin) const
{
    auto score = [](const Skin& skin) -> int
    {
        if (skin.skinType == SkinType::Scuttlebug)
            return 1;
        else if (skin.skinType == SkinType::ScuttlebugBlue)
            return 2;
        else if (skin.skinType == SkinType::Bighead)
            return 4;
        else if (skin.skinType == SkinType::Beholder)
            return 8;
        return 0;
    }(skin);

    return score * context.wave;
}

SurvivalSpawnerContext SurvivalGameDirector::createInitialContext()
{
    return SurvivalSpawnerContext {};
}

SkinType SurvivalGameDirector::getEnemyToSpawn() const
{
    return [](int currentWave, int enemiesSpawnedInThisWave) -> SkinType
    {
        if (currentWave == 1)
            return SkinType::Scuttlebug;
        else if (currentWave == 2)
        {
            if (enemiesSpawnedInThisWave % 3 == 0) return SkinType::Bighead;
            return SkinType::Scuttlebug;
        }
        else if (currentWave == 3)
        {
            if (enemiesSpawnedInThisWave % 2 == 0)
                return SkinType::Scuttlebug;
            else if (enemiesSpawnedInThisWave % 4 == 3)
                return SkinType::ScuttlebugBlue;
            return SkinType::Bighead;
        }

        if (enemiesSpawnedInThisWave % 2 == 0)
            return SkinType::Scuttlebug;
        else if (enemiesSpawnedInThisWave % 8 == 1)
            return SkinType::Bighead;
        else if (enemiesSpawnedInThisWave % 8 == 5)
            return SkinType::Beholder;
        return SkinType::ScuttlebugBlue;
    }(context.wave, context.enemiesSpawnedInCurrentWave);
}

int SurvivalGameDirector::getEnemyCountForWave(int wave)
{
    if (wave <= 6) return wave * 10;
    if (wave <= 13) return wave * 15;
    return wave * 20;
}
