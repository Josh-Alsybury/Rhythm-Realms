#pragma once
#include <SFML/Graphics.hpp>

struct EnemyTextures
{
    sf::Texture idle;
    sf::Texture run;
    sf::Texture attack;
    sf::Texture defend;

    bool load();
};


extern EnemyTextures g_enemyTextures;

