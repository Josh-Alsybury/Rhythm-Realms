#pragma once
#include <SFML/Graphics.hpp>

// Textures for Samurai (Enemy1)
struct SamuraiTextures
{
    sf::Texture idle;
    sf::Texture run;
    sf::Texture attack;
    sf::Texture defend;
    bool load();
};

// Textures for Archer (Enemy2)
struct ArcherTextures
{
    sf::Texture idle;
    sf::Texture run;
    sf::Texture attack;
    sf::Texture defend;
    bool load();
};

// Global texture instances
extern SamuraiTextures g_samuraiTextures;
extern ArcherTextures g_archerTextures;