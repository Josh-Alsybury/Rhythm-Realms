#pragma once
#include <SFML/Graphics.hpp>
#include <optional>

class Portal
{
public:
    enum class State { Closed, Opening, Active, Closing };

    Portal();

    void load(const sf::Texture& tex,
        sf::Vector2f worldPos,
        int columns = 8,
        int rows = 3,
        float frameTime = 0.08f);

    void update(float dt);
    void render(sf::RenderWindow& window, const sf::Vector2f& cameraOffset);

    void startOpening();
    void startClosing();

    bool isActive()  const { return m_state == State::Active; }
    bool isOpening() const { return m_state == State::Opening; }
    bool isClosing() const { return m_state == State::Closing; }
    bool isClosed()  const { return m_state == State::Closed; }

    sf::Vector2f getPosition() const { return m_worldPos; }

    int getDebugState() const { return static_cast<int>(m_state); }

private:
    const sf::Texture* m_texture = nullptr;
    std::optional<sf::Sprite>  m_sprite;  

    int   m_cols = 8;
    int   m_rows = 3;
    int   m_frameWidth = 0;
    int   m_frameHeight = 0;

    float m_frameTime = 0.08f;
    float m_timeAccum = 0.f;
    int   m_currentFrame = 0;

    State        m_state = State::Closed;
    sf::Vector2f m_worldPos;

    void setFrame(int column, int row);
};
