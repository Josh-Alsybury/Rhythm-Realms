#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include "Player.h"
#pragma once


class Npc
{
public:
	sf::Texture texture;
	sf::Sprite sprite{ texture };
	sf::Vector2f pos;

	sf::Vector2f velocity{ 0.f, 0.f };   
	sf::Vector2f acceleration{ 0.f, 0.f }; 

private:

	
};