#include "EnemyTextures.h"

// Global instances
SamuraiTextures g_samuraiTextures;
ArcherTextures g_archerTextures;

bool SamuraiTextures::load()
{
    return idle.loadFromFile("ASSETS/IMAGES/Samurai2/IDLE.png")
        && run.loadFromFile("ASSETS/IMAGES/Samurai2/RUN.png")
        && attack.loadFromFile("ASSETS/IMAGES/Samurai2/ATTACK1.png")
        && defend.loadFromFile("ASSETS/IMAGES/Samurai2/DEFENCE.png");
}

bool ArcherTextures::load()
{
    return idle.loadFromFile("ASSETS/IMAGES/Samurai_Archer/IDLE.png")
        && run.loadFromFile("ASSETS/IMAGES/Samurai_Archer/RUN.png")
        && attack.loadFromFile("ASSETS/IMAGES/Samurai_Archer/ATTACK.png")
        && defend.loadFromFile("ASSETS/IMAGES/Samurai_Archer/BLOCK.png");
}