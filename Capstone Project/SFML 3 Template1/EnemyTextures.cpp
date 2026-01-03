#include "EnemyTextures.h"

EnemyTextures g_enemyTextures;

bool EnemyTextures::load()
{
    return idle.loadFromFile("ASSETS/IMAGES/Samurai2/IDLE.png")
        && run.loadFromFile("ASSETS/IMAGES/Samurai2/RUN.png")
        && attack.loadFromFile("ASSETS/IMAGES/Samurai2/ATTACK1.png")
        && defend.loadFromFile("ASSETS/IMAGES/Samurai2/DEFENCE.png");
}
