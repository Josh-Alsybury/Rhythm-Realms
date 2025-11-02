#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cstdint> 

class Background
{
public:
    Background(const std::string& filePath, float parallaxFactor, int layerIndex = 0);
    void render(sf::RenderWindow& window);
    void setOffset(const sf::Vector2f& offset);
    void setOpacity(float opacity);
    Background(Background&&) noexcept;
    Background& operator=(Background&&) noexcept;

private:
    sf::Texture BGtexture;
    sf::Sprite BGsprite{ BGtexture };
    float m_parallax;
    std::vector<sf::Sprite> BGsprites;
    float m_width;
    float m_scaleX;
    float m_scaleY;
    int m_layerIndex = 0;
    sf::RectangleShape BGrect;
    float m_opacity = 1.0f;
};
