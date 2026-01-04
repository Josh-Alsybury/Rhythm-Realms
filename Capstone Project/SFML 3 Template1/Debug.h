#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "Headers/Player.h"
#include "Enemy1.h"
#include "Chunk.h"

class Debug
{
public:
    static void DrawPlayerCollision(sf::RenderWindow& window, player& p, sf::Vector2f cameraOffset, float hitboxWidth, float hitboxHeight);
    static void DrawEnemyCollision(sf::RenderWindow& window, std::vector<Enemy1>& enemies, sf::Vector2f cameraOffset);
    static void DrawChunkCollision(sf::RenderWindow& window, std::vector<Chunk>& chunks, sf::Vector2f cameraOffset);
};

#endif