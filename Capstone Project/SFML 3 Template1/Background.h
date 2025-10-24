#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

class Background
{
public:
    Background(const std::string& filePath, float parallaxFactor);

    void render(sf::RenderWindow& window);

    void setOffset(const sf::Vector2f& offset);

    Background(const Background&) = delete;
    Background& operator=(const Background&) = delete;
    Background(Background&&) noexcept = default;
    Background& operator=(Background&&) noexcept = default;

private:
    sf::Texture BGtexture;
    sf::Sprite BGsprite{ BGtexture };
    float m_parallax;
    std::vector<sf::Sprite> BGsprites;
    float m_width;
    sf::RectangleShape BGrect;
};
