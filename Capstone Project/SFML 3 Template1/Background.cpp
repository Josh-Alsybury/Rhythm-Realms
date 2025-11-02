#include "Background.h"
#include <stdexcept>

Background::Background(const std::string& filePath, float parallaxFactor, int layerIndex)
    : m_parallax(parallaxFactor), m_layerIndex(layerIndex)
{
    if (!BGtexture.loadFromFile(filePath))
        throw std::runtime_error("Failed to load background texture!");

    sf::Vector2u texSize = BGtexture.getSize();
    bool isTallLayer = (texSize.y > 150);

    if (isTallLayer)
    {
        m_scaleX = 1000.f / texSize.x;
        m_scaleY = 800.f / texSize.y;
        m_width = 1000.f;
    }
    else
    {
        m_scaleX = 1000.f / texSize.x;
        m_scaleY = m_scaleX;
        m_width = 1000.f;
    }

    float scaledHeight = texSize.y * m_scaleY;

    BGtexture.setRepeated(true);
    BGsprites.reserve(2);

    for (int i = 0; i < 2; ++i)
    {
        BGsprites.emplace_back(BGtexture);
        BGsprites[i].setScale(sf::Vector2f(m_scaleX, m_scaleY));
        float yPos = isTallLayer ? 0.f : (800.f - scaledHeight);
        BGsprites[i].setPosition(sf::Vector2f(i * m_width, yPos));
        BGsprites[i].setColor(sf::Color(255, 255, 255, 255));
    }
}
//move constructo
Background::Background(Background&& other) noexcept
    : BGtexture(std::move(other.BGtexture)),
    m_parallax(other.m_parallax),
    m_width(other.m_width),
    m_scaleX(other.m_scaleX),
    m_scaleY(other.m_scaleY),
    m_layerIndex(other.m_layerIndex),
    m_opacity(other.m_opacity)
{
    // new texture reference
    BGsprites.reserve(other.BGsprites.size());
    for (size_t i = 0; i < other.BGsprites.size(); ++i)
    {
        BGsprites.emplace_back(BGtexture);  // Use texture
        BGsprites[i].setScale(other.BGsprites[i].getScale());
        BGsprites[i].setPosition(other.BGsprites[i].getPosition());
        BGsprites[i].setColor(other.BGsprites[i].getColor());
    }
}
// assigment constructor
Background& Background::operator=(Background&& other) noexcept
{
    if (this != &other)
    {
        BGtexture = std::move(other.BGtexture);
        m_parallax = other.m_parallax;
        m_width = other.m_width;
        m_scaleX = other.m_scaleX;
        m_scaleY = other.m_scaleY;
        m_layerIndex = other.m_layerIndex;
        m_opacity = other.m_opacity;

        // new texture reference
        BGsprites.clear();
        BGsprites.reserve(other.BGsprites.size());
        for (size_t i = 0; i < other.BGsprites.size(); ++i)
        {
            BGsprites.emplace_back(BGtexture);  // use texture
            BGsprites[i].setScale(other.BGsprites[i].getScale());
            BGsprites[i].setPosition(other.BGsprites[i].getPosition());
            BGsprites[i].setColor(other.BGsprites[i].getColor());
        }
    }
    return *this;
}


void Background::render(sf::RenderWindow& window)
{
    for (auto& sprite : BGsprites)
        window.draw(sprite);
}

void Background::setOffset(const sf::Vector2f& offset)
{
    float x = -offset.x * m_parallax;

    float yPos = BGsprites[0].getPosition().y;
    for (int i = 0; i < 2; ++i)
    {
        float spriteX = i * m_width + fmod(x, m_width);
        BGsprites[i].setPosition(sf::Vector2f(spriteX, yPos));
    }
}

void Background::setOpacity(float opacity)
{
    m_opacity = std::clamp(opacity, 0.0f, 1.0f);
    std::uint8_t alpha = static_cast<std::uint8_t>(m_opacity * 255);
    for (auto& sprite : BGsprites)
    {
        sprite.setColor(sf::Color(255, 255, 255, alpha));
    }
}
