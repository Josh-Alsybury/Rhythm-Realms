#include "Arrow.h"
#include <iostream>

sf::Texture Arrow::arrowTexture;
bool Arrow::textureLoaded = false;

Arrow::Arrow(sf::Vector2f startPos, bool movingRight, float speed)
    : position(startPos)
    , active(true)
    , sprite(arrowTexture)  // SFML 3.0: Must initialize sprite with texture
{
    // Load texture once (static)
    if (!textureLoaded)
    {
        if (arrowTexture.loadFromFile("ASSETS/IMAGES/Samurai_Archer/ARROW.png"))
        {
            textureLoaded = true;
            std::cout << "Arrow texture loaded: " << arrowTexture.getSize().x << "x" << arrowTexture.getSize().y << std::endl;
        }
        else
        {
            std::cerr << "Failed to load arrow texture!" << std::endl;
        }
    }

    sprite.setOrigin({ 24.f, 8.f });  // Center of 48x16 sprite
    sprite.setPosition(startPos);
    sprite.setScale({ 2.f, 2.f });  // Scale up

    // Flip if moving left
    if (!movingRight)
    {
        sprite.setScale({ -2.f, 2.f });
    }

    velocity.x = movingRight ? speed : -speed;
    velocity.y = 0.f;
}

void Arrow::Update(float dt)
{
    if (!active) return;

    position += velocity * dt;
    sprite.setPosition(position);
}

bool Arrow::IsOffScreen(float cameraX, float screenWidth)
{
    float arrowScreenX = position.x - cameraX;
    return (arrowScreenX < -100.f || arrowScreenX > screenWidth + 100.f);
}

sf::FloatRect Arrow::getBounds() const
{
    return sf::FloatRect({ position.x - 20.f, position.y - 8.f }, { 40.f, 16.f });
}