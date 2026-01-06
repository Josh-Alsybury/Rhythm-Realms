#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Chunk.h"

// Unified collision and ground detection for ALL enemy types
class EnemyCollision
{
public:
    // Generic enemy hitbox size (adjust per enemy type if needed)
    static constexpr float ENEMY_HITBOX_WIDTH = 40.f;
    static constexpr float ENEMY_HITBOX_HEIGHT = 60.f;

    // Check horizontal collision and prevent wall-walking
    static bool CheckHorizontalCollision(
        sf::Vector2f& pos,
        sf::Vector2f& velocity,
        const std::vector<Chunk>& chunks,
        float dt)
    {
        // Apply horizontal movement
        float oldX = pos.x;
        pos.x += velocity.x * dt;

        // Check collision at new position (SFML 3 syntax)
        sf::FloatRect hitbox(
            sf::Vector2f(pos.x - ENEMY_HITBOX_WIDTH / 2.f, pos.y - ENEMY_HITBOX_HEIGHT / 2.f),
            sf::Vector2f(ENEMY_HITBOX_WIDTH, ENEMY_HITBOX_HEIGHT)
        );

        // Check all four corners of hitbox
        for (auto& chunk : chunks)
        {
            if (chunk.isSolidTileWorld(hitbox.position.x, hitbox.position.y) ||
                chunk.isSolidTileWorld(hitbox.position.x + hitbox.size.x, hitbox.position.y) ||
                chunk.isSolidTileWorld(hitbox.position.x, hitbox.position.y + hitbox.size.y) ||
                chunk.isSolidTileWorld(hitbox.position.x + hitbox.size.x, hitbox.position.y + hitbox.size.y))
            {
                // Hit wall - revert position and stop
                pos.x = oldX;
                velocity.x = 0.f;
                return true; // Collision detected
            }
        }

        return false; // No collision
    }

    // Apply gravity and snap to ground (prevents floating, makes enemies fall)
    static bool ApplyGravityAndGround(
        sf::Vector2f& pos,
        sf::Vector2f& velocity,
        const std::vector<Chunk>& chunks,
        float dt,
        float gravity = 980.f)
    {
        float feetY = pos.y + 30.f; // Feet position

        // Check if currently on ground
        bool onGround = false;
        for (auto& chunk : chunks)
        {
            if (chunk.isSolidTileWorld(pos.x, feetY + 2.f)) // Check slightly below
            {
                // On ground - snap to it
                float chunkRelativeY = feetY - chunk.getPosition().y;
                int tileY = static_cast<int>(chunkRelativeY / 32.f);
                pos.y = chunk.getPosition().y + (tileY * 32.f) - 30.f;
                velocity.y = 0.f;
                onGround = true;
                break;
            }
        }

        // If not on ground, apply gravity
        if (!onGround)
        {
            velocity.y += gravity * dt;
            pos.y += velocity.y * dt;

            // Check if we hit ground while falling
            feetY = pos.y + 30.f;
            for (auto& chunk : chunks)
            {
                if (chunk.isSolidTileWorld(pos.x, feetY))
                {
                    // Hit ground - snap to it
                    float chunkRelativeY = feetY - chunk.getPosition().y;
                    int tileY = static_cast<int>(chunkRelativeY / 32.f);
                    pos.y = chunk.getPosition().y + (tileY * 32.f) - 30.f;
                    velocity.y = 0.f;
                    onGround = true;
                    break;
                }
            }
        }

        return onGround;
    }

    // OLD VERSION - Keep for compatibility but mark as deprecated
    static bool SnapToGround(
        sf::Vector2f& pos,
        const std::vector<Chunk>& chunks,
        float maxFallDistance = 100.f)
    {
        float feetY = pos.y + 30.f; // Feet position (adjust based on sprite)
        float checkY = feetY;

        // Check downward for ground
        for (float offset = 0.f; offset <= maxFallDistance; offset += 4.f)
        {
            checkY = feetY + offset;

            // Check if we hit solid ground
            for (auto& chunk : chunks)
            {
                if (chunk.isSolidTileWorld(pos.x, checkY))
                {
                    // Found ground! Snap to it
                    int tileY = static_cast<int>((checkY - chunk.getPosition().y) / 32.f);
                    pos.y = chunk.getPosition().y + (tileY * 32.f) - 30.f;
                    return true; // Grounded
                }
            }
        }

        return false; // No ground found (falling into void)
    }

    // Check if position is valid spawn location (on solid ground)
    static bool IsValidSpawnPosition(
        sf::Vector2f pos,
        const std::vector<Chunk>& chunks)
    {
        float feetY = pos.y + 30.f;

        // Check if there's ground beneath spawn point
        for (auto& chunk : chunks)
        {
            if (chunk.isSolidTileWorld(pos.x, feetY))
            {
                return true;
            }
        }

        return false;
    }

    // Find nearest valid ground Y position for given X coordinate
    static float FindGroundY(
        float x,
        float startY,
        const std::vector<Chunk>& chunks,
        float searchRange = 200.f)
    {
        // Search downward for ground
        for (float y = startY; y < startY + searchRange; y += 4.f)
        {
            for (auto& chunk : chunks)
            {
                if (chunk.isSolidTileWorld(x, y))
                {
                    // Found ground tile
                    int tileY = static_cast<int>((y - chunk.getPosition().y) / 32.f);
                    return chunk.getPosition().y + (tileY * 32.f) - 30.f;
                }
            }
        }

        // No ground found, return original Y
        return startY;
    }

    // Check if enemy is about to walk off a ledge (for AI)
    // Returns TRUE if there's a LEDGE (no ground ahead)
    static bool IsLedgeAhead(
        sf::Vector2f pos,
        bool facingRight,
        const std::vector<Chunk>& chunks,
        float lookAheadDistance = 50.f)
    {
        // Check position slightly ahead in the direction enemy is facing
        float checkX = facingRight ? pos.x + lookAheadDistance : pos.x - lookAheadDistance;
        float checkY = pos.y + 40.f; // Check below feet level

        // Check if there's solid ground ahead
        bool foundGround = false;
        for (auto& chunk : chunks)
        {
            // Check multiple points downward to be thorough
            for (float yOffset = 0.f; yOffset <= 64.f; yOffset += 16.f)
            {
                if (chunk.isSolidTileWorld(checkX, checkY + yOffset))
                {
                    foundGround = true;
                    break;
                }
            }
            if (foundGround) break;
        }

        // Return TRUE if NO ground found (ledge detected!)
        return !foundGround;
    }

    // ENHANCED: Check and stop at ledge (call this in AI)
    static bool CheckAndStopAtLedge(
        sf::Vector2f pos,
        bool facingRight,
        sf::Vector2f& velocity,
        const std::vector<Chunk>& chunks)
    {
        if (IsLedgeAhead(pos, facingRight, chunks, 45.f))
        {
            // Stop moving at ledge
            velocity.x = 0.f;
            return true; // Ledge detected
        }
        return false; // Safe to move
    }
};