#include "Chunk.h"
#include <fstream>
#include <unordered_set>
#include <iostream>
#include "json.hpp"

// Load chunk data from Tiled JSON file and initialize collision/rendering
bool Chunk::load(const std::string& file, const sf::Texture& tileset, int tileSize) {
    std::ifstream f(file);
    if (!f.is_open()) {
        std::cerr << "Failed to open chunk file: " << file << std::endl;
        return false;
    }

    nlohmann::json data;
    f >> data;

    // Store texture reference and dimensions
    m_tileset = &tileset;
    m_width = data["width"];
    m_height = data["height"];
    m_tileSize = tileSize;

    // Load tile data from JSON
    auto tiles = data["layers"][0]["data"];
    m_tiles.resize(m_width * m_height);
    for (int i = 0; i < tiles.size(); ++i) {
        m_tiles[i] = tiles[i];
    }

    // Define which tile IDs are solid for collision
    std::unordered_set<int> solidTileIds = { 3, 29 };

    // Build collision map based on solid tile IDs
    m_collisionTiles.resize(m_width * m_height, 0);
    for (int i = 0; i < m_tiles.size(); ++i) {
        if (solidTileIds.count(m_tiles[i]) > 0) {
            m_collisionTiles[i] = m_tiles[i];
        }
    }

    // Build vertex array for optimized rendering
    buildVertexArray();

    return true;
}

//Build vertex array for efficient GPU rendering (called once per chunk load)
void Chunk::buildVertexArray() {
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_vertices.resize(m_width * m_height * 6);  // 6 vertices per tile (2 triangles)

    int cols = m_tileset->getSize().x / m_tileSize;

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            int id = m_tiles[y * m_width + x];
            if (id == 0) continue;  // Skip empty tiles
            --id;  // Tiled IDs are 1-based

            // Calculate texture coordinates
            int tu = id % cols;
            int tv = id / cols;

            // Get pointer to this tile's vertices (6 vertices = 2 triangles)
            sf::Vertex* quad = &m_vertices[(x + y * m_width) * 6];

            // Calculate positions
            sf::Vector2f pos(x * m_tileSize, y * m_tileSize);
            sf::Vector2f texPos(tu * m_tileSize, tv * m_tileSize);

            // Triangle 1: Top-left corner
            quad[0].position = pos;
            quad[1].position = pos + sf::Vector2f(m_tileSize, 0);
            quad[2].position = pos + sf::Vector2f(0, m_tileSize);

            // Triangle 2: Bottom-right corner
            quad[3].position = pos + sf::Vector2f(m_tileSize, 0);
            quad[4].position = pos + sf::Vector2f(m_tileSize, m_tileSize);
            quad[5].position = pos + sf::Vector2f(0, m_tileSize);

            // Apply texture coordinates
            quad[0].texCoords = texPos;
            quad[1].texCoords = texPos + sf::Vector2f(m_tileSize, 0);
            quad[2].texCoords = texPos + sf::Vector2f(0, m_tileSize);
            quad[3].texCoords = texPos + sf::Vector2f(m_tileSize, 0);
            quad[4].texCoords = texPos + sf::Vector2f(m_tileSize, m_tileSize);
            quad[5].texCoords = texPos + sf::Vector2f(0, m_tileSize);
        }
    }
}


// Render chunk with camera offset (single draw call)
void Chunk::draw(sf::RenderTarget& target, sf::Vector2f cameraOffset) {
    sf::RenderStates states;
    states.texture = m_tileset;
    states.transform.translate(m_position - cameraOffset);
    target.draw(m_vertices, states);
}


// Clear all tile data and free memory (for chunk recycling)
void Chunk::clearTiles() {
    m_tiles.clear();
    m_collisionTiles.clear();
    m_vertices.clear();

    // Shrink vectors to free memory
    m_tiles.shrink_to_fit();
    m_collisionTiles.shrink_to_fit();
}

// ===== COLLISION DETECTION =====


//Get tile ID at local grid coordinates
int Chunk::getTileAt(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return -1;
    return m_collisionTiles[y * m_width + x];
}

//check if tile at local grid coordinates is solid
bool Chunk::isSolidTile(int x, int y) const {
    return getTileAt(x, y) > 0;
}


bool Chunk::isSolidTileWorld(float worldX, float worldY) const {
    // Convert world position to local tile coordinates
    int tileX = static_cast<int>((worldX - m_position.x) / m_tileSize);
    int tileY = static_cast<int>((worldY - m_position.y) / m_tileSize);

    return isSolidTile(tileX, tileY);
}
