#ifndef DYNAMIC_BACKGROUND_H
#define DYNAMIC_BACKGROUND_H
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "Background.h"

// Theme enum for unified background + chunk + tileset management
enum class GameTheme
{
	Forest,
	Medieval,
	Factory,
	Hub  
};

class DynamicBackground
{
public:
	DynamicBackground() = default;

	// Background methods (existing)
	bool loadtheme(const std::string& folderPath);
	void update(const sf::Vector2f& cameraOffset);
	void render(sf::RenderWindow& window);
	void transitionTo(const std::string& newFolderPath);

	//Static theme helper methods - Get paths for everything based on theme
	static GameTheme GetThemeFromBPM(float bpm);
	static std::string GetBackgroundPath(GameTheme theme);
	static std::string GetTilesetTexturePath(GameTheme theme);
	static std::string GetTilesetPath(GameTheme theme);
	static std::vector<std::string> GetChunkPaths(GameTheme theme);

	// Get/set current theme
	GameTheme getCurrentTheme() const { return m_currentGameTheme; }
	void setCurrentTheme(GameTheme theme) { m_currentGameTheme = theme; }

private:
	std::vector<Background> m_layers;
	std::vector<Background> m_newLayers;
	std::string m_currentTheme;
	std::string m_nextTheme;
	bool m_isTransitioning = false;
	float m_transitionTimer = 0.0f;
	float m_transitionDuration = 2.0f;

	// Track current game theme
	GameTheme m_currentGameTheme = GameTheme::Forest;
};
#endif