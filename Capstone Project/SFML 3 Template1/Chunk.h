#ifndef CHUNK_H
#define CHUNK_H
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_set>
#include <string>
class Chunk {
private:
    sf::VertexArray m_vertices;
    std::vector<int> m_tiles;
    std::vector<int> m_collisionTiles;
    const sf::Texture* m_tileset;
    int m_width, m_height, m_tileSize;
    sf::Vector2f m_position;
    std::string m_currentTilesetPath;  // NEW: Track which tileset (.tsj) is loaded
public:
    static std::unordered_set<int> loadSolidTilesFromTilesetCached(const std::string& path);

    // NEW: Updated load method with tileset path parameter
    bool load(const std::string& file, const sf::Texture& tileset, int tileSize, const std::string& tilesetPath);

    void draw(sf::RenderTarget& target, sf::Vector2f cameraOffset);

    int getTileAt(int x, int y) const;
    bool isSolidTile(int x, int y) const;
    bool isSolidTileWorld(float worldX, float worldY) const;
    void drawDebugCollision(sf::RenderTarget& target, sf::Vector2f cameraOffset);
    void buildVertexArray();
    void setPosition(sf::Vector2f pos) { m_position = pos; }
    sf::Vector2f getPosition() const { return m_position; }
    float getWidth() const { return m_width * m_tileSize; }
    void clearTiles();
};
#endif