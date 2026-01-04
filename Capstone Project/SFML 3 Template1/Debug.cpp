#include "Debug.h"

void Debug::DrawPlayerCollision(sf::RenderWindow& window, player& p, sf::Vector2f cameraOffset, float hitboxWidth, float hitboxHeight)
{
    sf::RectangleShape hitboxVis({ hitboxWidth, hitboxHeight });
    hitboxVis.setPosition({
        p.pos.x - hitboxWidth / 2.f - cameraOffset.x,
        p.pos.y - hitboxHeight / 10.f - cameraOffset.y
        });
    hitboxVis.setFillColor(sf::Color::Transparent);
    hitboxVis.setOutlineColor(sf::Color::Green);
    hitboxVis.setOutlineThickness(2.f);
    window.draw(hitboxVis);

    sf::CircleShape feetDot(5.f);
    feetDot.setOrigin({ 5.f, 5.f });
    feetDot.setPosition({ p.pos.x - cameraOffset.x, p.pos.y + 50.f - cameraOffset.y });
    feetDot.setFillColor(sf::Color::Green);
    window.draw(feetDot);
}

void Debug::DrawEnemyCollision(sf::RenderWindow& window, std::vector<Enemy1>& enemies, sf::Vector2f cameraOffset)
{
    for (auto& enemy : enemies)
    {
        if (enemy.health <= 0) continue;

        sf::CircleShape enemyAttackVis(enemy.attackHitboxRadius);
        enemyAttackVis.setOrigin({ enemy.attackHitboxRadius, enemy.attackHitboxRadius });
        enemyAttackVis.setPosition({
            enemy.attackHitbox.getPosition().x - cameraOffset.x,
            enemy.attackHitbox.getPosition().y - cameraOffset.y
            });
        enemyAttackVis.setFillColor(sf::Color(255, 0, 0, 100));
        enemyAttackVis.setOutlineColor(sf::Color::Red);
        enemyAttackVis.setOutlineThickness(2.f);
        window.draw(enemyAttackVis);

        sf::CircleShape enemyDetectionVis(enemy.detectionRange);
        enemyDetectionVis.setOrigin({ enemy.detectionRange, enemy.detectionRange });
        enemyDetectionVis.setPosition({
            enemy.pos.x - cameraOffset.x,
            enemy.pos.y - cameraOffset.y
            });
        enemyDetectionVis.setFillColor(sf::Color(255, 255, 0, 50));
        enemyDetectionVis.setOutlineColor(sf::Color::Yellow);
        enemyDetectionVis.setOutlineThickness(2.f);
        window.draw(enemyDetectionVis);
    }
}

void Debug::DrawChunkCollision(sf::RenderWindow& window, std::vector<Chunk>& chunks, sf::Vector2f cameraOffset)
{
    for (auto& chunk : chunks)
    {
        chunk.drawDebugCollision(window, cameraOffset);
    }
}