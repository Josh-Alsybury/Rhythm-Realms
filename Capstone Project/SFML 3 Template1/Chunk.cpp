#include "Chunk.h"
#include <fstream>
#include "json.hpp"

bool Chunk::load(const std::string& file, const sf::Texture& tileset, int tileSize) {
    std::ifstream f(file);
    if (!f.is_open()) return false;

    nlohmann::json data;
    f >> data;

    m_tileset = &tileset;
    m_width = data["width"];
    m_height = data["height"];
    m_tileSize = tileSize;

    // Get tiles
    auto tiles = data["layers"][0]["data"];
    m_tiles.resize(m_width * m_height);
    for (int i = 0; i < tiles.size(); ++i) {
        m_tiles[i] = tiles[i];
    }
    return true;
}

void Chunk::draw(sf::RenderTarget& target, sf::Vector2f cameraOffset) {
    int cols = m_tileset->getSize().x / m_tileSize;
    float yOffset = 190.0f; // Adjust this value to move the chunk up or down

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            int id = m_tiles[y * m_width + x];
            if (id == 0) continue;
            --id;

            sf::Sprite tile(*m_tileset);

            // Apply camera offset + vertical offset to position
            tile.setPosition(sf::Vector2f(
                x * m_tileSize - cameraOffset.x,
                y * m_tileSize + yOffset - cameraOffset.y  // Added yOffset here
            ));

            tile.setTextureRect(sf::IntRect(
                sf::Vector2i((id % cols) * m_tileSize, (id / cols) * m_tileSize),
                sf::Vector2i(m_tileSize, m_tileSize)
            ));

            target.draw(tile);
        }
    }
}

