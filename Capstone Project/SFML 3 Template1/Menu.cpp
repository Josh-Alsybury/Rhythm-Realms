#include "Menu.h"
#include <iostream>

Menu::Menu(sf::Font& font)
    : m_titleText(font)
    , m_localAudioButton(font)
    , m_spotifyButton(font)
{
    // Title
    m_titleText.setString("Select Audio Source");
    m_titleText.setCharacterSize(48);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setPosition(sf::Vector2f(250.f, 150.f));

    // Local Audio Button
    m_localAudioBox.setSize(sf::Vector2f(300.f, 80.f));
    m_localAudioBox.setPosition(sf::Vector2f(350.f, 300.f));
    m_localAudioBox.setFillColor(sf::Color(70, 70, 70));
    m_localAudioBox.setOutlineThickness(3.f);
    m_localAudioBox.setOutlineColor(sf::Color::White);

    m_localAudioButton.setString("Local Audio Files");
    m_localAudioButton.setCharacterSize(32);
    m_localAudioButton.setFillColor(sf::Color::White);
    m_localAudioButton.setPosition(sf::Vector2f(380.f, 320.f));

    // Spotify Button
    m_spotifyBox.setSize(sf::Vector2f(300.f, 80.f));
    m_spotifyBox.setPosition(sf::Vector2f(350.f, 450.f));
    m_spotifyBox.setFillColor(sf::Color(30, 215, 96)); // Spotify green
    m_spotifyBox.setOutlineThickness(3.f);
    m_spotifyBox.setOutlineColor(sf::Color::White);

    m_spotifyButton.setString("Spotify");
    m_spotifyButton.setCharacterSize(32);
    m_spotifyButton.setFillColor(sf::Color::White);
    m_spotifyButton.setPosition(sf::Vector2f(440.f, 470.f));
}

void Menu::HandleClick(sf::Vector2f mousePos)
{
    if (isMouseOver(m_localAudioBox, mousePos))
    {
        m_audioSourceSelected = AudioSource::Local;
        std::cout << "Local audio selected" << std::endl;
    }
    else if (isMouseOver(m_spotifyBox, mousePos))
    {
        m_audioSourceSelected = AudioSource::Spotify;
        std::cout << "Spotify selected" << std::endl;
    }
}

void Menu::Draw(sf::RenderWindow& window)
{
    window.draw(m_titleText);
    window.draw(m_localAudioBox);
    window.draw(m_localAudioButton);
    window.draw(m_spotifyBox);
    window.draw(m_spotifyButton);
}

bool Menu::isMouseOver(const sf::RectangleShape& box, sf::Vector2f mousePos)
{
    sf::FloatRect bounds = box.getGlobalBounds();
    return bounds.contains(mousePos);
}