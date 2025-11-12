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
#include "DynamicBackground.h"
#include "BpmStream.h"
#include "Enemy1.h"


const sf::Color WHITE{ 0, 0,0,0 }; // const colour

class Game
{
public:
	Game();
	~Game();
	void run();

	std::vector<Enemy1> m_enemies;
	int distance = 0;

private:
	DynamicBackground m_dynamicBackground;
	std::string m_currentTheme = "Forest";

	sf::Vector2f m_cameraOffset{ 100.f, 0.f };
	const float m_screenMargin = 200.f;

	void processEvents();
	void processKeys(const std::optional<sf::Event> t_event);
	void checkKeyboardState();
	void update(sf::Time t_deltaTime);
	void switchSong();
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
	//std::vector<Npc> m_npcs;
	sf::Text m_bpmText{ m_jerseyFont };
	BpmStream m_bpmStream;
	double m_currentBPM = 0.0;
	std::vector<std::string> m_songPaths;
	size_t m_currentSongIndex;

	mybpm::MiniBPM m_bpmAnalyzer{ static_cast<int>(44100) };
};

#pragma warning( pop ) 
#endif 
