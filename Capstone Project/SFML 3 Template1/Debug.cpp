#include "Debug.h"

void Debug::DrawPlayerCollision(
    sf::RenderWindow& window,
    player& p,
    sf::Vector2f cameraOffset,
    float hitboxWidth,
    float hitboxHeight)
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
    feetDot.setPosition({
        p.pos.x - cameraOffset.x,
        p.pos.y + 50.f - cameraOffset.y
        });
    feetDot.setFillColor(sf::Color::Green);
    window.draw(feetDot);
}

void Debug::DrawEnemy1Collision(
    sf::RenderWindow& window,
    std::vector<Enemy1>& enemies,
    sf::Vector2f cameraOffset)
{
    for (auto& enemy : enemies)
    {
        if (enemy.health <= 0) continue;

        // Draw enemy hitbox (red for melee)
        DrawEnemyHitbox(window, enemy.pos, cameraOffset, 40.f, 60.f, sf::Color::Red);

        // Draw detection and attack ranges
        DrawEnemyRanges(
            window,
            enemy.pos,
            cameraOffset,
            enemy.detectionRange,
            enemy.attackRange,
            sf::Color(255, 255, 0, 50),   // Yellow detection
            sf::Color(255, 0, 0, 100)     // Red attack
        );

        // Draw attack hitbox if attacking
        if (enemy.canDamagePlayer)
        {
            sf::CircleShape attackHitboxVis(enemy.attackHitboxRadius);
            attackHitboxVis.setOrigin({ enemy.attackHitboxRadius, enemy.attackHitboxRadius });
            attackHitboxVis.setPosition({
                enemy.attackHitbox.getPosition().x - cameraOffset.x,
                enemy.attackHitbox.getPosition().y - cameraOffset.y
                });
            attackHitboxVis.setFillColor(sf::Color(255, 0, 0, 150));
            attackHitboxVis.setOutlineColor(sf::Color::Red);
            attackHitboxVis.setOutlineThickness(2.f);
            window.draw(attackHitboxVis);
        }
    }
}

void Debug::DrawEnemy2Collision(
    sf::RenderWindow& window,
    std::vector<Enemy2>& archers,
    sf::Vector2f cameraOffset)
{
    for (auto& archer : archers)
    {
        if (archer.health <= 0) continue;

        // Draw archer hitbox (purple for archers)
        DrawEnemyHitbox(window, archer.pos, cameraOffset, 40.f, 60.f, sf::Color(150, 0, 255));

        // Draw detection and attack ranges (different colors for archer)
        DrawEnemyRanges(
            window,
            archer.pos,
            cameraOffset,
            archer.detectionRange,
            archer.attackRange,
            sf::Color(255, 200, 0, 50),    // Gold detection (longer range)
            sf::Color(150, 0, 255, 100)    // Purple attack
        );

        // Draw attack hitbox (arrow release zone)
        if (archer.canDamagePlayer)
        {
            sf::CircleShape attackHitboxVis(archer.attackHitboxRadius);
            attackHitboxVis.setOrigin({ archer.attackHitboxRadius, archer.attackHitboxRadius });
            attackHitboxVis.setPosition({
                archer.attackHitbox.getPosition().x - cameraOffset.x,
                archer.attackHitbox.getPosition().y - cameraOffset.y
                });
            attackHitboxVis.setFillColor(sf::Color(150, 0, 255, 150));
            attackHitboxVis.setOutlineColor(sf::Color(150, 0, 255));
            attackHitboxVis.setOutlineThickness(2.f);
            window.draw(attackHitboxVis);
        }

        // Draw preferred distance ring (archer-specific)
        sf::CircleShape preferredVis(archer.preferredDistance);
        preferredVis.setOrigin({ archer.preferredDistance, archer.preferredDistance });
        preferredVis.setPosition({ archer.pos.x - cameraOffset.x, archer.pos.y - cameraOffset.y });
        preferredVis.setFillColor(sf::Color::Transparent);
        preferredVis.setOutlineColor(sf::Color(255, 255, 255, 100));
        preferredVis.setOutlineThickness(1.f);
        window.draw(preferredVis);
    }
}

void Debug::DrawChunkCollision(
    sf::RenderWindow& window,
    std::vector<Chunk>& chunks,
    sf::Vector2f cameraOffset)
{
    for (auto& chunk : chunks)
    {
        chunk.drawDebugCollision(window, cameraOffset);
    }
}

void Debug::DrawShopSlots(
    sf::RenderWindow& window,
    const std::array<ShopSlotUI, 6>& slots)
{
    for (const auto& slot : slots)
    {
        sf::RectangleShape rect;
        rect.setSize(slot.area.size);               
        rect.setPosition(slot.area.position);       
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineColor(sf::Color::Yellow);
        rect.setOutlineThickness(2.f);
        window.draw(rect);
    }
}
