#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <random>
#include "Enemy1.h"
#include "Enemy2.h"
#include "Enemy3.h"

// Forward declaration
class Chunk;

// Spawn configuration for different enemy types
struct EnemySpawnConfig {
    float minSpawnDistance;      // Min distance from player to spawn
    float maxSpawnDistance;      // Max distance from player to spawn
    float spawnCooldown;         // Time between spawns
    int maxActiveEnemies;        // Max enemies of this type on screen

    EnemySpawnConfig(float minDist = 800.f, float maxDist = 1200.f,
        float cooldown = 3.0f, int maxActive = 3)
        : minSpawnDistance(minDist), maxSpawnDistance(maxDist),
        spawnCooldown(cooldown), maxActiveEnemies(maxActive) {}
};

// Track enemy spawn positions to avoid clustering ( sort of works its awkward tho )
struct SpawnRecord {
    float worldX;
    float timestamp;
    SpawnRecord(float x, float time) : worldX(x), timestamp(time) {}
};

class EnemySpawnManager {
public:
    EnemySpawnManager();

    // Configuration
    void SetSpawnConfig(const EnemySpawnConfig& config);
    void SetDifficultyMultiplier(float multiplier); // Increases spawn rate over time

    // Update and spawn logic
    void Update(float dt, sf::Vector2f playerPos,
        std::vector<Enemy1>& enemies,
        std::vector<Enemy2>& archers,
        std::vector<Enemy3>& executioners,  
        float rightmostChunkX,
        const std::vector<Chunk>& chunks);

    // Manual spawn
    void ForceSpawn(sf::Vector2f position, std::vector<Enemy1>& enemies);
    void ForceSpawnArcher(sf::Vector2f position, std::vector<Enemy2>& archers);
    void ForceSpawnExecutioner(sf::Vector2f position, std::vector<Enemy3>& executioners);  

    // Getters for UI/stats
    int GetTotalEnemiesSpawned() const { return totalEnemiesSpawned; }
    int GetActiveEnemyCount() const { return activeEnemyCount; }
    float GetNextSpawnIn() const { return spawnCooldownTimer; }

    // Debug rendering
    void DrawDebugInfo(sf::RenderWindow& window, sf::Vector2f cameraOffset, const sf::Font& font);

private:
    EnemySpawnConfig config;
    EnemySpawnConfig archerConfig;
    EnemySpawnConfig executionerConfig;  

    // Spawn state
    float spawnCooldownTimer;
    float archerSpawnCooldownTimer;
    float executionerSpawnCooldownTimer;  
    float difficultyMultiplier;
    int totalEnemiesSpawned;
    int totalArchersSpawned;
    int totalExecutionersSpawned;  
    int activeEnemyCount;
    int activeArcherCount;
    int activeExecutionerCount;  

    // Random generation
    std::mt19937 rng;
    std::uniform_real_distribution<float> distanceDistribution;
    std::uniform_real_distribution<float> heightVariation;

    // Spawn history
    std::vector<SpawnRecord> recentSpawns;
    const float MIN_SPAWN_SEPARATION = 300.f;
    const float SPAWN_RECORD_DURATION = 10.f; // Keep records for 10 seconds

    // Helper methods
    float GetRandomSpawnX(sf::Vector2f playerPos, float rightmostChunkX);
    float GetSpawnY(float spawnX, const std::vector<Chunk>& chunks, float searchStartY = 500.f);
    bool CanSpawnAt(float worldX);
    void CleanupOldSpawnRecords(float currentTime);
    bool IsValidSpawnPosition(float worldX, float playerX, float rightmostChunkX);
};