#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class Menu
{
public:
    enum class AudioSource
    {
        None,
        Local,
        Spotify
    };

    Menu(sf::Font& font);
    void HandleClick(sf::Vector2f mousePos);
    void Draw(sf::RenderWindow& window);
    bool IsComplete() const { return m_audioSourceSelected != AudioSource::None; }
    AudioSource GetSelectedSource() const { return m_audioSourceSelected; }

private:
    sf::Text m_titleText;
    sf::Text m_localAudioButton;
    sf::Text m_spotifyButton;
    sf::RectangleShape m_localAudioBox;
    sf::RectangleShape m_spotifyBox;

    AudioSource m_audioSourceSelected = AudioSource::None;

    bool isMouseOver(const sf::RectangleShape& box, sf::Vector2f mousePos);
};