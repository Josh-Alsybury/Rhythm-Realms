#ifndef DYNAMIC_BACKGROUND_H
#define DYNAMIC_BACKGROUND_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "Background.h"

class DynamicBackground
{
public:

	DynamicBackground() = default;
	bool loadtheme(const std::string& folderPath);
	void update(const sf::Vector2f& cameraOffset);
	void render(sf::RenderWindow& window);
	void transitionTo(const std::string& newFolderPath);

private:
	std::vector<Background> m_layers;
	std::vector<Background> m_newLayers;
	std::string m_currentTheme;
	std::string m_nextTheme;
	bool m_isTransitioning = false;
	float m_transitionTimer = 0.0f;
	float m_transitionDuration = 2.0f;
};


#endif