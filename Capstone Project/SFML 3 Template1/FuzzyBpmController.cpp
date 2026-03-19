#include "FuzzyBpmController.h"

FuzzyBpmController::FuzzyBpmController()
{
    m_lowParams.enemySpeed = 80.f;
    m_lowParams.enemyHP = 7.f;
    m_lowParams.attackCooldown = 2.0f;
    m_lowParams.spawnRate =6.0f;
    m_lowParams.attackWindowSize = 1.4f;  

    m_mediumParams.enemySpeed = 130.f;
    m_mediumParams.enemyHP = 5.f;
    m_mediumParams.attackCooldown = 1.5f;
    m_mediumParams.spawnRate = 3.5f;
    m_mediumParams.attackWindowSize = 1.0f;

    m_highParams.enemySpeed = 200.f;
    m_highParams.enemyHP = 2.f;
    m_highParams.attackCooldown = 0.6f;
    m_highParams.spawnRate = 1.0f;
    m_highParams.attackWindowSize = 0.7f;
}

float FuzzyBpmController::membershipLow(float bpm)
{
    if (bpm <= 90.f) return 1.f;
    if (bpm >= 110.f) return 0.f;
    return (110.f - bpm) / 20.f;  
}

float FuzzyBpmController::membershipMedium(float bpm)
{
    if (bpm <= 90.f || bpm >= 150.f) return 0.f;
    if (bpm <= 120.f) return (bpm - 90.f) / 30.f;
    return (150.f - bpm) / 30.f;
}

float FuzzyBpmController::membershipHigh(float bpm)
{
    if (bpm <= 120.f) return 0.f;
    if (bpm >= 150.f) return 1.f;
    return (bpm - 120.f) / 20.f;
}

FuzzyGameParams FuzzyBpmController::defuzzify(float low, float medium, float high)
{
    float total = low + medium + high;

    if (total < 0.001f)
        return m_mediumParams;

    FuzzyGameParams result;
    result.enemySpeed = (low * m_lowParams.enemySpeed + medium * m_mediumParams.enemySpeed + high * m_highParams.enemySpeed) / total;
    result.enemyHP = (low * m_lowParams.enemyHP + medium * m_mediumParams.enemyHP + high * m_highParams.enemyHP) / total;
    result.attackCooldown = (low * m_lowParams.attackCooldown + medium * m_mediumParams.attackCooldown + high * m_highParams.attackCooldown) / total;
    result.spawnRate = (low * m_lowParams.spawnRate + medium * m_mediumParams.spawnRate + high * m_highParams.spawnRate) / total;
    result.attackWindowSize = (low * m_lowParams.attackWindowSize + medium * m_mediumParams.attackWindowSize + high * m_highParams.attackWindowSize) / total;

    return result;
}

FuzzyGameParams FuzzyBpmController::update(float bpm)
{
    float low = membershipLow(bpm);
    float medium = membershipMedium(bpm);
    float high = membershipHigh(bpm);

    return defuzzify(low, medium, high);
}
