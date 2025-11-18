#include "Chunk.h"
#include <fstream>
#include <unordered_set>
#include <iostream>
#include "json.hpp"

std::unordered_set<int> Chunk::loadSolidTilesFromTilesetCached(const std::string& path) {
    static std::unordered_map<std::string, std::unordered_set<int>> s_cache;

    auto it = s_cache.find(path);
    if (it != s_cache.end())
        return it->second;

    std::unordered_set<int> result;

    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "Failed to open tileset: " << path << std::endl;
        return result;
    }

    nlohmann::json tilesetData;
    f >> tilesetData;

    if (tilesetData.contains("tiles")) {
        for (auto& tile : tilesetData["tiles"]) {
            int tileId = tile["id"].get<int>() + 1;
            if (tile.contains("properties")) {
                for (auto& prop : tile["properties"]) {
                    if (prop["name"] == "solid" && prop["value"] == true) {
                        result.insert(tileId);
                    }
                }
            }
        }
    }

    s_cache[path] = result;
    return result;
}

// Load chunk data from Tiled JSON file and initialize collision/rendering
bool Chunk::load(const std::string& file, const sf::Texture& tileset, int tileSize) {

    clearTiles();

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
    std::unordered_set<int> solidTileIds = Chunk::loadSolidTilesFromTilesetCached("ASSETS/Tiles/Forest.tsj");

    // Fallback to hardcoded if tileset loading failed
    if (solidTileIds.empty()) {
        std::cout << "Warning: No solid tiles loaded from tileset, using defaults" << std::endl;
        solidTileIds = { 3, 29 };
    }  // CLOSE THE IF STATEMENT HERE

    // Build collision map 
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
    m_vertices.clear();
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);

    // First pass: count non-empty tiles
    int nonEmptyTiles = 0;
    for (int id : m_tiles) if (id != 0) ++nonEmptyTiles;
    m_vertices.resize(nonEmptyTiles * 6);

    // Allocate exact space needed (6 vertices per non-empty tile)
    m_vertices.resize(nonEmptyTiles * 6);

    int cols = m_tileset->getSize().x / m_tileSize;
    const float epsilon = 0.1f;

    int vertexIndex = 0;  // Track current vertex position

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            int id = m_tiles[y * m_width + x];

            // CRITICAL: Skip empty tiles completely
            if (id == 0) continue;

            --id;  // Tiled IDs are 1-based

            // Calculate texture coordinates
            int tu = id % cols;
            int tv = id / cols;

            // Get pointer to this tile's vertices
            sf::Vertex* quad = &m_vertices[vertexIndex];
            vertexIndex += 6;  // Move to next tile's vertices

            // Calculate positions
            sf::Vector2f pos(x * m_tileSize, y * m_tileSize);
            sf::Vector2f texPos(tu * m_tileSize + epsilon, tv * m_tileSize + epsilon);
            sf::Vector2f texSize(m_tileSize - 2 * epsilon, m_tileSize - 2 * epsilon);

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
            quad[1].texCoords = texPos + sf::Vector2f(texSize.x, 0);
            quad[2].texCoords = texPos + sf::Vector2f(0, texSize.y);
            quad[3].texCoords = texPos + sf::Vector2f(texSize.x, 0);
            quad[4].texCoords = texPos + texSize;
            quad[5].texCoords = texPos + sf::Vector2f(0, texSize.y);
        }
    }
}


// Render chunk with camera offset (single draw call)
void Chunk::draw(sf::RenderTarget& target, sf::Vector2f cameraOffset) {
    
    if (m_vertices.getVertexCount() == 0) {
        return;  // Nothing to draw
    }

    sf::RenderStates states;
    states.texture = m_tileset;

    // Round to integer pixel positions
    sf::Vector2f renderPos = m_position - cameraOffset; //This prevents subpixel rendering that causes texture bleeding.
    renderPos.x = std::round(renderPos.x);
    renderPos.y = std::round(renderPos.y);

    states.transform.translate(renderPos);
    target.draw(m_vertices, states);
}


// Clear all tile data and free memory (for chunk recycling)
void Chunk::clearTiles() {
    m_tiles.clear();
    m_collisionTiles.clear();
    m_vertices.clear();
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
