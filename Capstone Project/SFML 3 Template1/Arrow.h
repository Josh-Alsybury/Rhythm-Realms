#pragma once
#include <SFML/Graphics.hpp>

class Arrow
{
public:
    Arrow(sf::Vector2f startPos, bool movingRight, float speed = 400.f);

    void Update(float dt);
    bool IsOffScreen(float cameraX, float screenWidth);

    sf::Vector2f getPosition() const { return position; }
    sf::FloatRect getBounds() const;

    sf::Sprite sprite;
    bool active;

private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    static sf::Texture arrowTexture;
    static bool textureLoaded;
};