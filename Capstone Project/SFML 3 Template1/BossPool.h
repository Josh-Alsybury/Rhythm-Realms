#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "Enemy2.h"
#include "Enemy3.h"

enum class BossType
{
    Archer,       // Enemy2 boss
    Executioner   // Enemy3 boss
};

class Pool
{
public:
    Pool()
    {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
    }

    BossType PickBoss()
    {
        return (std::rand() % 2 == 0) ? BossType::Archer : BossType::Executioner;
    }

    Enemy2 SpawnArcherBoss(sf::Vector2f spawnPos)
    {
        Enemy2 boss;
        boss.SetupEnemy2();
        boss.pos = spawnPos;
        boss.SetAsBoss();
        return boss;
    }

    Enemy3 SpawnExecutionerBoss(sf::Vector2f spawnPos)
    {
        Enemy3 boss;
        boss.SetupEnemy3();
        boss.pos = spawnPos;
        boss.SetAsBoss();
        return boss;
    }
};