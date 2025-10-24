/// <summary>
/// author Pete Lowe May 2025
/// you need to change the above line or lose marks
/// Also don't have any member properties called Delete...
/// </summary>
#ifndef GAME_HPP
#define GAME_HPP
#pragma warning( push )
#pragma warning( disable : 4275 )
// ignore this warning
// C:\SFML - 3.0.0\include\SFML\System\Exception.hpp(41, 47) : 
// warning C4275 : non dll - interface class 'std::runtime_error' used as base for dll - interface class 'sf::Exception'

/// <summary>
/// include guards used so we don't process this file twice
/// same as #pragma once
/// Don't forget the #endif at the bottom
/// </summary>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.h"
#include "Npc.h"
#include "Background.h"
#include "BPM.h"



const sf::Color WHITE{ 0, 0,0,0 }; // const colour

class Game
{
public:
	Game();
	~Game();
	void run();

	sf::ConvexShape cone;

private:
	
	BPM m_bpmAnalyzer;
	float m_basePlayerSpeed = 250.f;
	float m_baseScrollSpeed = 100.f;

	sf::Vector2f m_cameraOffset{ 100.f, 0.f };
	const float m_screenMargin = 200.f;

	void processEvents();
	void processKeys(const std::optional<sf::Event> t_event);
	void checkKeyboardState();
	void update(sf::Time t_deltaTime);
	void render();
	
	void setupTexts();
	void setupSprites();
	void setupAudio();
	void initNPCs();

	sf::RenderWindow m_window; // main SFML window
	sf::Font m_jerseyFont;// font used by message
	sf::Text m_formationHintText{ m_jerseyFont };

	bool m_DELETEexitGame; // control exiting game

	player m_Player;
	std::vector<Npc> m_npcs;
	std::vector<Background> m_backgroundLayers;
	
};

#pragma warning( pop ) 
#endif // !GAME_HPP

