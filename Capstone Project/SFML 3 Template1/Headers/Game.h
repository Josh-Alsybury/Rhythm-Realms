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
#include "Chunk.h"
#include "Player.h"
#include "DynamicBackground.h"
#include "BpmStream.h"
#include "EnemyTextures.h"
#include "Enemy1.h"
#include "SkillTree.h"
#include "Menu.h"
#include "SpotifyClient.h"
#include "EnemySpawnManager.h"
#include "Debug.h"


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
	void initializeGame();
	void update(sf::Time t_deltaTime);
	void switchSong();
	void render();
	
	void setupTexts();
	void setupSprites();
	void setupAudio();

	const float PLAYER_HITBOX_WIDTH = 30.f;
	const float PLAYER_HITBOX_HEIGHT = 40.f;

	sf::Font m_jerseyFont;
	std::unique_ptr<Menu> m_mainMenu;  // Use pointer so we can initialize after font loads
	bool m_showMenu = true;
	Menu::AudioSource m_selectedAudioSource = Menu::AudioSource::None;

	// Spotify
	SpotifyClient m_spotifyClient;
	bool m_useSpotify = false;

	sf::RenderWindow m_window; // main SFML window;
	sf::Text m_formationHintText{ m_jerseyFont };

	void updateChunks();  //Manages chunk loading/unloading
	bool loadChunkAt(int index, float xPosition);  // Loads chunk at position
	std::vector<std::string> m_chunkPaths;
	std::vector<Chunk> m_chunks;  // from single Chunk to vector
	const int VISIBLE_CHUNKS = 3;  // NNumber of chunks to keep loaded
	float m_chunkWidth = 640.0f;   // Width of each chunk (20 tiles * 32px)
	int m_nextChunkIndex = 0;
	sf::Texture m_tilesetTexture;

	bool m_showDebugCollision = false;

	// Enemy spawning
	EnemySpawnManager m_enemySpawnManager;
	float m_gameTimer;  // Tracks total game time

	// Spawn configurations for different difficulties
	EnemySpawnConfig m_easyConfig;
	EnemySpawnConfig m_normalConfig;
	EnemySpawnConfig m_hardConfig;
	
	bool m_DELETEexitGame; // control exiting game
	player m_Player;
	//std::vector<Npc> m_npcs;
	sf::Text m_bpmText{ m_jerseyFont };
	BpmStream m_bpmStream;
	SkillTree m_skillTree;
	double m_currentBPM = 0.0;
	bool m_showSkillTree = false;
	std::vector<std::string> m_songPaths;
	size_t m_currentSongIndex;

	mybpm::MiniBPM m_bpmAnalyzer{ static_cast<int>(44100) };
};

#pragma warning( pop ) 
#endif 
