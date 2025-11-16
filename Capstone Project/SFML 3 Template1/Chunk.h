#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Chunk {
public:
    bool load(const std::string& file, const sf::Texture& tileset, int tileSize);
    void draw(sf::RenderTarget& target, sf::Vector2f cameraOffset);
private:
    const sf::Texture* m_tileset = nullptr;
    std::vector<int> m_tiles;
    int m_width = 0;
    int m_height = 0;
    int m_tileSize = 0;
};
