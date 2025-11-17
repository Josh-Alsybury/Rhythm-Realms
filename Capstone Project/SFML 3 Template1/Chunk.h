#ifndef CHUNK_H
#define CHUNK_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_set>
#include <string>

class Chunk {
private:
    std::unordered_set<int> loadSolidTilesFromTileset(const std::string& tilesetPath);

    sf::VertexArray m_vertices;
    std::vector<int> m_tiles;
    std::vector<int> m_collisionTiles;
    const sf::Texture* m_tileset;
    int m_width, m_height, m_tileSize;
    sf::Vector2f m_position;

public:
    bool load(const std::string& file, const sf::Texture& tileset, int tileSize);
    void draw(sf::RenderTarget& target, sf::Vector2f cameraOffset);
    
    int getTileAt(int x, int y) const;
    bool isSolidTile(int x, int y) const;
    bool isSolidTileWorld(float worldX, float worldY) const;

    void buildVertexArray();

    void setPosition(sf::Vector2f pos) { m_position = pos; }
    sf::Vector2f getPosition() const { return m_position; }
    float getWidth() const { return m_width * m_tileSize; }
    void clearTiles();
};

#endif
