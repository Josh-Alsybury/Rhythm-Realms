#pragma once

/// <summary>
/// Fuzzy Logic BPM Controller
/// Takes live BPM as input and outputs game parameters
/// </summary>

struct FuzzyGameParams
{
	float enemySpeed;
	float enemyHP;
	float attackCooldown;
	float spawnRate;
	float attackWindowSize;
	float xpMultiplier;
};



class FuzzyBpmController
{
public:
	FuzzyBpmController();

	FuzzyGameParams update(float bpm);

	float membershipLow(float bpm);
	float membershipMedium(float bpm);
	float membershipHigh(float bpm);

private:
	FuzzyGameParams defuzzify(float low, float medium, float high);

	FuzzyGameParams m_lowParams;
	FuzzyGameParams m_mediumParams;
	FuzzyGameParams m_highParams;
};

