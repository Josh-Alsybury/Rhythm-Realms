#include "Background.h"
#include <stdexcept>

Background::Background(const std::string& filePath, float parallaxFactor)
    : m_parallax(parallaxFactor)
{
    if (!BGtexture.loadFromFile(filePath))
        throw std::runtime_error("Failed to load background texture!");

    sf::Vector2u texSize = BGtexture.getSize();
    float scaleX = 1000.f / texSize.x;
    float scaleY = 800.f / texSize.y;
    m_width = texSize.x * scaleX;

    BGtexture.setRepeated(true);
    BGsprites.reserve(2); 
    for (int i = 0; i < 2; ++i)
    {
        BGsprites.emplace_back(BGtexture); 
        BGsprites[i].setScale(sf::Vector2f(scaleX, scaleY));
        BGsprites[i].setPosition(sf::Vector2f(i * m_width, 0.f));
    }
}

void Background::render(sf::RenderWindow& window)
{
    for (auto& sprite : BGsprites)
        window.draw(sprite);
}

void Background::setOffset(const sf::Vector2f& offset)
{
    float x = -offset.x * m_parallax;

    for (int i = 0; i < 2; ++i)
    {
        float spriteX = i * m_width + fmod(x, m_width);
        BGsprites[i].setPosition(sf::Vector2f(spriteX, 0.f));
    }
}
