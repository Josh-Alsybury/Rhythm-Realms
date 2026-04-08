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
    sf::Vector2f velocity;

    void SetVelocity(sf::Vector2f vel) { velocity = vel; }
    sf::Vector2f GetPosition() const { return position; }

private:
    sf::Vector2f position;
    static sf::Texture arrowTexture;
    static bool textureLoaded;
};