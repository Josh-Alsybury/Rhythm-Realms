#include "Hub.h"
#include "Headers/DynamicBackground.h"
#include "Debug.h"
#include <iostream>

Hub::Hub()
    : m_startExpedition(false)
{
}

void Hub::Load(
    const sf::Texture& tileset,
    std::vector<Chunk>& chunks,
    player& player,
    float& chunkWidth)
{
    std::cout << "=== LOADING HUB ===" << std::endl;

    auto hubChunks = DynamicBackground::GetChunkPaths(GameTheme::Hub);
    chunks.clear();
    chunks.resize(hubChunks.size());

    std::string hubTilesetPath =
        DynamicBackground::GetTilesetPath(GameTheme::Hub);

    float xPos = 0.f;

    for (int i = 0; i < hubChunks.size(); ++i)
    {
        if (!chunks[i].load(hubChunks[i], tileset, 32, hubTilesetPath))
        {
            std::cout << "Failed to load hub chunk " << i << std::endl;
            continue;
        }

        chunks[i].setPosition({ xPos, 190.f });
        xPos += chunks[i].getWidth();
    }

    chunkWidth = chunks[0].getWidth();

    // Player spawn
    player.pos = { 320.f, 400.f };

    std::cout << "=== HUB LOADED ===" << std::endl;
}


bool Hub::Update(float dt, const player& player)
{
    // Check if player wants to start expedition
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
    {
        m_startExpedition = true;
        std::cout << "Player pressed E - starting expedition!" << std::endl;
        return true;  // Signal to Game.cpp to switch to expedition
    }

    return false;
}

void Hub::Render(
    sf::RenderWindow& window,
    std::vector<Chunk>& chunks,
    player& player,
    const sf::Vector2f& cameraOffset,
    bool showDebugCollision,
    float PLAYER_HITBOX_WIDTH,
    float PLAYER_HITBOX_HEIGHT)
{
    // Render hub chunks (background IS in the chunks)
    for (auto& chunk : chunks)
    {
        chunk.draw(window, cameraOffset);
    }

    // Debug collision (if enabled)
    if (showDebugCollision)
    {
        Debug::DrawChunkCollision(window, chunks, cameraOffset);
        Debug::DrawPlayerCollision(window, player, cameraOffset,
            PLAYER_HITBOX_WIDTH, PLAYER_HITBOX_HEIGHT);
    }

    // Render player
    window.draw(*player.sprite);

    // Render player UI
    for (auto& bar : player.HealBar)
        window.draw(bar);
    for (int i = 0; i < player.HealsCount; i++)
        window.draw(player.HealSphere[i]);
}