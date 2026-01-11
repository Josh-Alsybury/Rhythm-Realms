#include "Portal.h"

Portal::Portal() = default;


void Portal::load(const sf::Texture& tex,
    sf::Vector2f worldPos,
    int columns,
    int rows,
    float frameTime)
{
    m_texture = &tex;
    m_cols = columns;
    m_rows = rows;
    m_frameTime = frameTime;

    m_frameWidth = static_cast<int>(tex.getSize().x) / m_cols;
    m_frameHeight = static_cast<int>(tex.getSize().y) / m_rows;

    m_worldPos = worldPos;

    m_sprite.emplace(tex);

    m_currentFrame = 0;

    // Always start active instead of closed
    m_state = State::Active;
    setFrame(0, 0); // first frame of active row (top)

    m_sprite->setOrigin({ m_frameWidth / 2.f, m_frameHeight / 2.f });
    m_sprite->setScale({ 2.f, 2.f });
    m_sprite->setPosition(m_worldPos);
}

void Portal::setFrame(int column, int row)
{
    if (!m_sprite) return;

    sf::Vector2i pos(column * m_frameWidth, row * m_frameHeight);
    sf::Vector2i size(m_frameWidth, m_frameHeight);
    sf::IntRect rect(pos, size);   

    m_sprite->setTextureRect(rect);
}


void Portal::startOpening()
{
    if (!m_sprite) return;
    if (m_state == State::Closed || m_state == State::Closing) {
        m_state = State::Opening;
        m_timeAccum = 0.f;
        m_currentFrame = 0;
        setFrame(0, 1);
    }
}

void Portal::startClosing()
{
    if (!m_sprite) return;
    if (m_state == State::Active) {
        m_state = State::Closing;
        m_timeAccum = 0.f;
        m_currentFrame = 0;
        setFrame(0, 2);
    }
}

void Portal::update(float dt)
{
    if (m_state == State::Closed)
        return;

    m_timeAccum += dt;

    while (m_timeAccum >= m_frameTime)
    {
        m_timeAccum -= m_frameTime;
        m_currentFrame++;

        switch (m_state)
        {
        case State::Opening:
            if (m_currentFrame >= m_cols)
            {
                // finished opening -> become active and start looping active row
                m_state = State::Active;
                m_currentFrame = 0;
                setFrame(0, 0); // active row (top)
            }
            else
            {
                setFrame(m_currentFrame, 1); // row 1 = opening
            }
            break;

        case State::Active:
            // loop top row
            m_currentFrame %= m_cols;
            setFrame(m_currentFrame, 0); // row 0 = active
            break;

        case State::Closing:
            if (m_currentFrame >= m_cols)
            {
                // finished closing -> fully closed
                m_state = State::Closed;
                m_currentFrame = 0;
                setFrame(0, 2); // keep first closed frame
            }
            else
            {
                setFrame(m_currentFrame, 2); // row 2 = closing
            }
            break;

        default:
            break;
        }
    }
}

void Portal::render(sf::RenderWindow& window, const sf::Vector2f& cameraOffset)
{
    if (m_state == State::Closed)
        return;

    sf::Sprite sprite = *m_sprite;
    sprite.move(-cameraOffset);
    window.draw(sprite);
}
