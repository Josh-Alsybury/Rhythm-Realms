#ifndef HUB_H
#define HUB_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Chunk.h"
#include "Headers/Player.h"

// Simple Hub class - handles hub loading, updating, and rendering
class Hub
{
public:
    Hub();

    // Load hub world
    void Load(
        const sf::Texture& tileset,
        std::vector<Chunk>& chunks,
        player& player,
        float& chunkWidth);

    // Update hub (checks for E key press)
    bool Update(float dt, const player& player);  // Returns true if player wants to start expedition

    // Render hub
    void Render(
        sf::RenderWindow& window,
        std::vector<Chunk>& chunks,
        player& player,
        const sf::Vector2f& cameraOffset,
        bool showDebugCollision,
        float PLAYER_HITBOX_WIDTH,
        float PLAYER_HITBOX_HEIGHT);

private:
    bool m_startExpedition;
};

#endif