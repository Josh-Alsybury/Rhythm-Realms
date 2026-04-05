#include "EnemyTextures.h"

// Global instances
SamuraiTextures g_samuraiTextures;
ArcherTextures g_archerTextures;
ExecutionerTextures g_executionerTextures;

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

bool ExecutionerTextures::load()
{
    return idle.loadFromFile("ASSETS/IMAGES/Executioner/IDLE.png")
        && walk.loadFromFile("ASSETS/IMAGES/Executioner/WALK.png")
        && attack1.loadFromFile("ASSETS/IMAGES/Executioner/ATTACK 1.png")
        && attack2.loadFromFile("ASSETS/IMAGES/Executioner/ATTACK 2.png");
}
