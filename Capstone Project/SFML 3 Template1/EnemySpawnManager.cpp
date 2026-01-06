#include "EnemySpawnManager.h"
#include <iostream>
#include <cmath>
#include "Chunk.h"
#include "EnemyCollision.h"

EnemySpawnManager::EnemySpawnManager()
    : spawnCooldownTimer(0.f)
    , archerSpawnCooldownTimer(0.f)
    , difficultyMultiplier(1.0f)
    , totalEnemiesSpawned(0)
    , totalArchersSpawned(0)
    , activeEnemyCount(0)
    , activeArcherCount(0)
    , rng(std::random_device{}())
    , distanceDistribution(800.f, 1200.f)
    , heightVariation(-20.f, 20.f)
{
    config = EnemySpawnConfig(); // Default for melee enemies
    archerConfig = EnemySpawnConfig(900.f, 1500.f, 5.0f, 2); // Archers spawn further, less frequently
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
    difficultyMultiplier = std::max(0.5f, std::min(multiplier, 3.0f));
}

void EnemySpawnManager::Update(float dt, sf::Vector2f playerPos,
    std::vector<Enemy1>& enemies, std::vector<Enemy2>& archers,
    float rightmostChunkX, const std::vector<Chunk>& chunks)  // ADDED CHUNKS PARAMETER
{
    // Update cooldowns
    spawnCooldownTimer -= dt;
    archerSpawnCooldownTimer -= dt;

    // Clean up old spawn records
    static float timeSinceStart = 0.f;
    timeSinceStart += dt;
    CleanupOldSpawnRecords(timeSinceStart);

    // Count active enemies
    activeEnemyCount = 0;
    for (const auto& enemy : enemies) {
        if (enemy.health > 0) {
            activeEnemyCount++;
        }
    }

    activeArcherCount = 0;
    for (const auto& archer : archers) {
        if (archer.health > 0) {
            activeArcherCount++;
        }
    }

    // ========== MELEE ENEMY SPAWNING ==========
    bool shouldSpawnMelee = spawnCooldownTimer <= 0.f
        && activeEnemyCount < config.maxActiveEnemies;

    if (shouldSpawnMelee)
    {
        float spawnX = GetRandomSpawnX(playerPos, rightmostChunkX);

        if (CanSpawnAt(spawnX))
        {
            bool spawned = false;
            for (auto& enemy : enemies)
            {
                if (enemy.health <= 0)
                {
                    float spawnY = GetSpawnY(spawnX, chunks);  // FIXED: Added both parameters

                    enemy.SetupEnemy1();
                    enemy.Reset();
                    enemy.pos = { spawnX, spawnY };
                    enemy.sprite->setPosition(enemy.pos);

                    recentSpawns.emplace_back(spawnX, timeSinceStart);
                    totalEnemiesSpawned++;
                    spawned = true;

                    std::cout << "Recycled melee enemy at X: " << spawnX << std::endl;
                    break;
                }
            }

            if (!spawned && enemies.size() < static_cast<size_t>(config.maxActiveEnemies))
            {
                enemies.emplace_back();
                auto& newEnemy = enemies.back();
                newEnemy.SetupEnemy1();

                float spawnY = GetSpawnY(spawnX, chunks);  // FIXED: Added both parameters
                newEnemy.pos = { spawnX, spawnY };
                newEnemy.sprite->setPosition(newEnemy.pos);

                recentSpawns.emplace_back(spawnX, timeSinceStart);
                totalEnemiesSpawned++;

                std::cout << "Created new melee enemy at X: " << spawnX << std::endl;
            }

            float adjustedCooldown = config.spawnCooldown / difficultyMultiplier;
            spawnCooldownTimer = adjustedCooldown;
        }
        else
        {
            spawnCooldownTimer = 0.5f;
        }
    }

    // ========== ARCHER SPAWNING ==========
    bool shouldSpawnArcher = archerSpawnCooldownTimer <= 0.f
        && activeArcherCount < archerConfig.maxActiveEnemies;

    if (shouldSpawnArcher)
    {
        // Use archer's longer spawn distance
        std::uniform_real_distribution<float> archerDistDist(
            archerConfig.minSpawnDistance, archerConfig.maxSpawnDistance
        );

        float archerSpawnDistance = archerDistDist(rng);
        float archerSpawnX = playerPos.x + archerSpawnDistance;

        // Clamp to chunk bounds
        float maxSpawnX = rightmostChunkX - 200.f;
        if (archerSpawnX > maxSpawnX)
            archerSpawnX = maxSpawnX;

        if (CanSpawnAt(archerSpawnX))
        {
            bool spawned = false;
            for (auto& archer : archers)
            {
                if (archer.health <= 0)
                {
                    float spawnY = GetSpawnY(archerSpawnX, chunks);  // FIXED: Added both parameters

                    archer.SetupEnemy2();
                    archer.Reset();
                    archer.pos = { archerSpawnX, spawnY };
                    archer.sprite->setPosition(archer.pos);

                    recentSpawns.emplace_back(archerSpawnX, timeSinceStart);
                    totalArchersSpawned++;
                    spawned = true;

                    std::cout << "Recycled archer at X: " << archerSpawnX << std::endl;
                    break;
                }
            }

            if (!spawned && archers.size() < static_cast<size_t>(archerConfig.maxActiveEnemies))
            {
                archers.emplace_back();
                auto& newArcher = archers.back();
                newArcher.SetupEnemy2();
                newArcher.Reset();

                float spawnY = GetSpawnY(archerSpawnX, chunks);  // FIXED: Added both parameters
                newArcher.pos = { archerSpawnX, spawnY };
                newArcher.sprite->setPosition(newArcher.pos);

                recentSpawns.emplace_back(archerSpawnX, timeSinceStart);
                totalArchersSpawned++;

                std::cout << "Created new archer at X: " << archerSpawnX << std::endl;
            }

            float adjustedCooldown = archerConfig.spawnCooldown / difficultyMultiplier;
            archerSpawnCooldownTimer = adjustedCooldown;
        }
        else
        {
            archerSpawnCooldownTimer = 0.5f;
        }
    }
}

void EnemySpawnManager::ForceSpawn(sf::Vector2f position, std::vector<Enemy1>& enemies)
{
    for (auto& enemy : enemies)
    {
        if (enemy.health <= 0)
        {
            enemy.SetupEnemy1();
            enemy.Reset();
            enemy.pos = position;
            enemy.sprite->setPosition(enemy.pos);

            totalEnemiesSpawned++;
            std::cout << "Force spawned melee enemy at (" << position.x << ", " << position.y << ")" << std::endl;
            return;
        }
    }

    if (enemies.size() < static_cast<size_t>(config.maxActiveEnemies))
    {
        enemies.emplace_back();
        auto& newEnemy = enemies.back();
        newEnemy.SetupEnemy1();
        newEnemy.pos = position;
        newEnemy.sprite->setPosition(newEnemy.pos);
        totalEnemiesSpawned++;

        std::cout << "Force spawned NEW melee enemy at (" << position.x << ", " << position.y << ")" << std::endl;
    }
}

void EnemySpawnManager::ForceSpawnArcher(
    sf::Vector2f position,
    std::vector<Enemy2>& archers)
{
    for (auto& archer : archers)
    {
        if (archer.health <= 0)
        {
            archer.SetupEnemy2();
            archer.Reset();
            archer.pos = position;
            archer.sprite->setPosition(archer.pos);

            totalArchersSpawned++;
            std::cout << "Force spawned archer at ("
                << position.x << ", " << position.y << ")\n";
            return;
        }
    }

    // --- CAP CHECK ---
    if (archers.size() >= static_cast<size_t>(archerConfig.maxActiveEnemies))
        return;

    // --- CREATE NEW ARCHER ---
    archers.emplace_back();
    auto& newArcher = archers.back();

    newArcher.SetupEnemy2();   // textures + sprite
    newArcher.Reset();         // reset state
    newArcher.pos = position;
    newArcher.sprite->setPosition(newArcher.pos);

    totalArchersSpawned++;
    std::cout << "Force spawned NEW archer at ("
        << position.x << ", " << position.y << ")\n";
}

float EnemySpawnManager::GetRandomSpawnX(sf::Vector2f playerPos, float rightmostChunkX)
{
    float spawnDistance = distanceDistribution(rng);
    float potentialSpawnX = playerPos.x + spawnDistance;

    float maxSpawnX = rightmostChunkX - 200.f;

    if (potentialSpawnX > maxSpawnX)
    {
        potentialSpawnX = maxSpawnX;
    }

    return potentialSpawnX;
}

float EnemySpawnManager::GetSpawnY(float spawnX, const std::vector<Chunk>& chunks)
{
    // Start searching from a base height
    float searchStartY = 500.f;

    // Use unified collision system to find ground
    float groundY = EnemyCollision::FindGroundY(spawnX, searchStartY, chunks, 300.f);

    return groundY;
}

bool EnemySpawnManager::CanSpawnAt(float worldX)
{
    for (const auto& record : recentSpawns)
    {
        float distance = std::abs(worldX - record.worldX);
        if (distance < MIN_SPAWN_SEPARATION)
        {
            return false;
        }
    }

    return true;
}

void EnemySpawnManager::CleanupOldSpawnRecords(float currentTime)
{
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
    if (worldX < playerX + config.minSpawnDistance)
        return false;

    if (worldX > playerX + config.maxSpawnDistance)
        return false;

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

    std::string info = "Melee: " + std::to_string(activeEnemyCount) + "/"
        + std::to_string(config.maxActiveEnemies);
    info += " | Archers: " + std::to_string(activeArcherCount) + "/"
        + std::to_string(archerConfig.maxActiveEnemies);
    info += "\nTotal Spawned: " + std::to_string(totalEnemiesSpawned + totalArchersSpawned);
    info += "\nNext Melee: " + std::to_string(static_cast<int>(spawnCooldownTimer)) + "s";
    info += " | Next Archer: " + std::to_string(static_cast<int>(archerSpawnCooldownTimer)) + "s";
    info += "\nDifficulty: " + std::to_string(difficultyMultiplier).substr(0, 4) + "x";

    debugText.setString(info);
    window.draw(debugText);
}