#include "EnemySpawnManager.h"
#include <iostream>
#include <cmath>

EnemySpawnManager::EnemySpawnManager()
    : spawnCooldownTimer(0.f)
    , difficultyMultiplier(1.0f)
    , totalEnemiesSpawned(0)
    , activeEnemyCount(0)
    , rng(std::random_device{}())
    , distanceDistribution(800.f, 1200.f)
    , heightVariation(-20.f, 20.f)
{
    config = EnemySpawnConfig(); // Use default values
}

void EnemySpawnManager::SetSpawnConfig(const EnemySpawnConfig& newConfig)
{
    config = newConfig;

    // Update distribution ranges
    distanceDistribution = std::uniform_real_distribution<float>(
        config.minSpawnDistance, config.maxSpawnDistance
    );
}

void EnemySpawnManager::SetDifficultyMultiplier(float multiplier)
{
    difficultyMultiplier = std::max(0.5f, std::min(multiplier, 3.0f)); // Clamp between 0.5x and 3x
}

void EnemySpawnManager::Update(float dt, sf::Vector2f playerPos,
    std::vector<Enemy1>& enemies, float rightmostChunkX)
{
    // Update cooldown
    spawnCooldownTimer -= dt;

    // Clean up old spawn records
    static float timeSinceStart = 0.f;
    timeSinceStart += dt;
    CleanupOldSpawnRecords(timeSinceStart);

    // Count active enemies (alive ones)
    activeEnemyCount = 0;
    for (const auto& enemy : enemies) {
        if (enemy.health > 0) {
            activeEnemyCount++;
        }
    }

    // Check if we should spawn
    bool shouldSpawn = spawnCooldownTimer <= 0.f
        && activeEnemyCount < config.maxActiveEnemies;

    if (shouldSpawn)
    {
        // Get random spawn position ahead of player
        float spawnX = GetRandomSpawnX(playerPos, rightmostChunkX);

        // Check if position is valid (not too close to recent spawns)
        if (CanSpawnAt(spawnX))
        {
            // Find a dead/recyclable enemy slot
            bool spawned = false;
            for (auto& enemy : enemies)
            {
                if (enemy.health <= 0)
                {
                    // Recycle this enemy
                    float spawnY = GetSpawnY();

                    // Use Reset() method to properly reinitialize
                    enemy.Reset();
                    enemy.SetupEnemy1();

                    // Set new position
                    enemy.pos = { spawnX, spawnY };
                    enemy.sprite->setPosition(enemy.pos);

                    // Record spawn
                    recentSpawns.emplace_back(spawnX, timeSinceStart);
                    totalEnemiesSpawned++;
                    spawned = true;

                    std::cout << "Recycled enemy at X: " << spawnX << " (Total: "
                        << totalEnemiesSpawned << ")" << std::endl;
                    break;
                }
            }

            // If no recyclable slot found and we have room, create new enemy
            if (!spawned && enemies.size() < static_cast<size_t>(config.maxActiveEnemies))
            {
                enemies.emplace_back();
                auto& newEnemy = enemies.back();
                newEnemy.SetupEnemy1();

                float spawnY = GetSpawnY();
                newEnemy.pos = { spawnX, spawnY };
                newEnemy.sprite->setPosition(newEnemy.pos);

                // Record spawn
                recentSpawns.emplace_back(spawnX, timeSinceStart);
                totalEnemiesSpawned++;

                std::cout << "Created new enemy at X: " << spawnX << " (Total: "
                    << totalEnemiesSpawned << ")" << std::endl;
            }

            // Reset cooldown (affected by difficulty)
            float adjustedCooldown = config.spawnCooldown / difficultyMultiplier;
            spawnCooldownTimer = adjustedCooldown;
        }
        else
        {
            // Try again in a short time if position was invalid
            spawnCooldownTimer = 0.5f;
        }
    }
}

void EnemySpawnManager::ForceSpawn(sf::Vector2f position, std::vector<Enemy1>& enemies)
{
    // Find dead enemy to recycle or create new one
    for (auto& enemy : enemies)
    {
        if (enemy.health <= 0)
        {
            enemy.Reset();
            enemy.pos = position;
            enemy.sprite->setPosition(enemy.pos);
            enemy.SetupEnemy1();

            totalEnemiesSpawned++;
            std::cout << "Force spawned enemy at (" << position.x << ", " << position.y << ")" << std::endl;
            return;
        }
    }

    // No recyclable slot, create new
    if (enemies.size() < static_cast<size_t>(config.maxActiveEnemies))
    {
        enemies.emplace_back();
        auto& newEnemy = enemies.back();
        newEnemy.SetupEnemy1();
        newEnemy.pos = position;
        newEnemy.sprite->setPosition(newEnemy.pos);
        totalEnemiesSpawned++;

        std::cout << "Force spawned NEW enemy at (" << position.x << ", " << position.y << ")" << std::endl;
    }
}

float EnemySpawnManager::GetRandomSpawnX(sf::Vector2f playerPos, float rightmostChunkX)
{
    // Spawn somewhere ahead of player but within loaded chunks
    float spawnDistance = distanceDistribution(rng);
    float potentialSpawnX = playerPos.x + spawnDistance;

    // Make sure we don't spawn beyond the rightmost chunk
    float maxSpawnX = rightmostChunkX - 200.f; // Leave some margin

    if (potentialSpawnX > maxSpawnX)
    {
        potentialSpawnX = maxSpawnX;
    }

    return potentialSpawnX;
}

float EnemySpawnManager::GetSpawnY()
{
    // Ground level is around 746-760 based on your game
    float baseGroundY = 746.f;
    float variation = heightVariation(rng);
    return baseGroundY + variation;
}

bool EnemySpawnManager::CanSpawnAt(float worldX)
{
    // Check if too close to recent spawns
    for (const auto& record : recentSpawns)
    {
        float distance = std::abs(worldX - record.worldX);
        if (distance < MIN_SPAWN_SEPARATION)
        {
            return false; // Too close to a recent spawn
        }
    }

    return true;
}

void EnemySpawnManager::CleanupOldSpawnRecords(float currentTime)
{
    // Remove spawn records older than SPAWN_RECORD_DURATION
    recentSpawns.erase(
        std::remove_if(recentSpawns.begin(), recentSpawns.end(),
            [currentTime, this](const SpawnRecord& record) {
                return (currentTime - record.timestamp) > SPAWN_RECORD_DURATION;
            }),
        recentSpawns.end()
    );
}

bool EnemySpawnManager::IsValidSpawnPosition(float worldX, float playerX, float rightmostChunkX)
{
    // Must be ahead of player
    if (worldX < playerX + config.minSpawnDistance)
        return false;

    // Must not be too far ahead
    if (worldX > playerX + config.maxSpawnDistance)
        return false;

    // Must be within loaded chunks
    if (worldX > rightmostChunkX - 200.f)
        return false;

    return true;
}

void EnemySpawnManager::DrawDebugInfo(sf::RenderWindow& window,
    sf::Vector2f cameraOffset, const sf::Font& font)
{
    sf::Text debugText{ font };
    debugText.setCharacterSize(20);
    debugText.setFillColor(sf::Color::Yellow);
    debugText.setPosition({ 50.f, 120.f });

    std::string info = "Enemies: " + std::to_string(activeEnemyCount) + "/"
        + std::to_string(config.maxActiveEnemies);
    info += "\nTotal Spawned: " + std::to_string(totalEnemiesSpawned);
    info += "\nNext Spawn: " + std::to_string(static_cast<int>(spawnCooldownTimer)) + "s";
    info += "\nDifficulty: " + std::to_string(difficultyMultiplier).substr(0, 4) + "x";

    debugText.setString(info);
    window.draw(debugText);
}