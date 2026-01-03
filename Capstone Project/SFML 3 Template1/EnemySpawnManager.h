#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <random>
#include "Enemy1.h"

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

// Track enemy spawn positions to avoid clustering
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
    void Update(float dt, sf::Vector2f playerPos, std::vector<Enemy1>& enemies, 
                float rightmostChunkX);
    
    // Manual spawn (for special events)
    void ForceSpawn(sf::Vector2f position, std::vector<Enemy1>& enemies);
    
    // Getters for UI/stats
    int GetTotalEnemiesSpawned() const { return totalEnemiesSpawned; }
    int GetActiveEnemyCount() const { return activeEnemyCount; }
    float GetNextSpawnIn() const { return spawnCooldownTimer; }
    
    // Debug rendering
    void DrawDebugInfo(sf::RenderWindow& window, sf::Vector2f cameraOffset, const sf::Font& font);
    
private:
    EnemySpawnConfig config;
    
    // Spawn state
    float spawnCooldownTimer;
    float difficultyMultiplier;
    int totalEnemiesSpawned;
    int activeEnemyCount;
    
    // Random generation
    std::mt19937 rng;
    std::uniform_real_distribution<float> distanceDistribution;
    std::uniform_real_distribution<float> heightVariation;
    
    // Spawn history (prevent clustering)
    std::vector<SpawnRecord> recentSpawns;
    const float MIN_SPAWN_SEPARATION = 300.f;
    const float SPAWN_RECORD_DURATION = 10.f; // Keep records for 10 seconds
    
    // Helper methods
    float GetRandomSpawnX(sf::Vector2f playerPos, float rightmostChunkX);
    float GetSpawnY(); // Ground level with slight variation
    bool CanSpawnAt(float worldX);
    void CleanupOldSpawnRecords(float currentTime);
    bool IsValidSpawnPosition(float worldX, float playerX, float rightmostChunkX);
};
