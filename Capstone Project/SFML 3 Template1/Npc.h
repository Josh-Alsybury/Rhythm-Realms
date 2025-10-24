#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include "Player.h"
#pragma once

struct  SterringOutput
{
	sf::Vector2f linear;
	float angualr = 0.0f;
};

class Npc
{
public:
	sf::Texture texture;
	sf::Sprite sprite{ texture };
	sf::Vector2f pos;

	sf::Vector2f velocity{ 0.f, 0.f };   
	sf::Vector2f acceleration{ 0.f, 0.f }; 
	float maxSpeed = 200.0f;             
	float maxAcceleration = 100.0f;
	float rotation = 0.0f;

	float angularVelocity = 0.0f;
	float maxRotation = 180.0f;      
	float maxAngularAcc = 90.0f;
	float wanderAngle = 0.f;

	float visionLength = 150.f;
	float visionAngle = 60.f;

	void SetupNpc()
	{
		

		sprite.setTexture(texture, true);
		sprite.setScale(sf::Vector2f{ 0.5f, 0.5f });

		pos = { 800, 300 };
		velocity = { 0, 0 };

		sprite.setPosition(pos);

		auto bounds = sprite.getLocalBounds();
		sprite.setOrigin(bounds.size / 2.0f);
	}
	 

	SterringOutput Wander();

	SterringOutput ArriveToSlot(const sf::Vector2f& targetPos, float slowRadius, float arrivalRadius);
	
	void Update(const SterringOutput& steering, float deltaTime);
	void wrapAround(sf::Vector2f& pos, float screenWidth, float screenHeight);

private:
	float MapToRange(float angle);

	
};