#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "Headers/Player.h"
#include "Enemy1.h"
#include "Enemy2.h"
#include "Chunk.h"

class Debug
{
public:
    // Player collision visualization
    static void DrawPlayerCollision(
        sf::RenderWindow& window,
        player& p,
        sf::Vector2f cameraOffset,
        float hitboxWidth,
        float hitboxHeight);

    // Melee enemy (Enemy1) collision visualization
    static void DrawEnemy1Collision(
        sf::RenderWindow& window,
        std::vector<Enemy1>& enemies,
        sf::Vector2f cameraOffset);

    // Archer (Enemy2) collision visualization
    static void DrawEnemy2Collision(
        sf::RenderWindow& window,
        std::vector<Enemy2>& archers,
        sf::Vector2f cameraOffset);

    // Draw ALL enemies at once (convenience method)
    static void DrawAllEnemiesCollision(
        sf::RenderWindow& window,
        std::vector<Enemy1>& enemies,
        std::vector<Enemy2>& archers,
        sf::Vector2f cameraOffset)
    {
        DrawEnemy1Collision(window, enemies, cameraOffset);
        DrawEnemy2Collision(window, archers, cameraOffset);
    }

    // Chunk collision visualization
    static void DrawChunkCollision(
        sf::RenderWindow& window,
        std::vector<Chunk>& chunks,
        sf::Vector2f cameraOffset);

private:
    // Helper: Draw generic enemy hitbox (reusable for all enemy types)
    static void DrawEnemyHitbox(
        sf::RenderWindow& window,
        sf::Vector2f pos,
        sf::Vector2f cameraOffset,
        float width = 40.f,
        float height = 60.f,
        sf::Color color = sf::Color::Red)
    {
        sf::RectangleShape hitbox({ width, height });
        hitbox.setPosition({
            pos.x - width / 2.f - cameraOffset.x,
            pos.y - height / 2.f - cameraOffset.y
            });
        hitbox.setFillColor(sf::Color::Transparent);
        hitbox.setOutlineColor(color);
        hitbox.setOutlineThickness(2.f);
        window.draw(hitbox);

        // Draw feet position
        sf::CircleShape feetDot(4.f);
        feetDot.setOrigin({ 4.f, 4.f });
        feetDot.setPosition({
            pos.x - cameraOffset.x,
            pos.y + height / 2.f - cameraOffset.y
            });
        feetDot.setFillColor(color);
        window.draw(feetDot);
    }

    // Helper: Draw detection/attack ranges
    static void DrawEnemyRanges(
        sf::RenderWindow& window,
        sf::Vector2f pos,
        sf::Vector2f cameraOffset,
        float detectionRange,
        float attackRange,
        sf::Color detectionColor = sf::Color(255, 255, 0, 50),
        sf::Color attackColor = sf::Color(255, 0, 0, 100))
    {
        // Detection range
        sf::CircleShape detectionVis(detectionRange);
        detectionVis.setOrigin({ detectionRange, detectionRange });
        detectionVis.setPosition({ pos.x - cameraOffset.x, pos.y - cameraOffset.y });
        detectionVis.setFillColor(detectionColor);
        detectionVis.setOutlineColor(sf::Color(detectionColor.r, detectionColor.g, detectionColor.b, 255));
        detectionVis.setOutlineThickness(1.f);
        window.draw(detectionVis);

        // Attack range
        sf::CircleShape attackVis(attackRange);
        attackVis.setOrigin({ attackRange, attackRange });
        attackVis.setPosition({ pos.x - cameraOffset.x, pos.y - cameraOffset.y });
        attackVis.setFillColor(attackColor);
        attackVis.setOutlineColor(sf::Color(attackColor.r, attackColor.g, attackColor.b, 255));
        attackVis.setOutlineThickness(2.f);
        window.draw(attackVis);
    }
};

#endif