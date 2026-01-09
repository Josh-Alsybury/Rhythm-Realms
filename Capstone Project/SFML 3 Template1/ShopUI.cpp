#include "ShopUI.h"
#include "Debug.h"

ShopUI::ShopUI() = default;

void ShopUI::load(const sf::Texture& cabinetTex,
    const sf::Font& /*font*/,
    const sf::Vector2u& windowSize)
{
    m_cabinetTexture = &cabinetTex;

    m_overlay.setSize(sf::Vector2f(windowSize));
    m_overlay.setFillColor(sf::Color(0, 0, 0, 200));

    // construct sprite now that we have a texture
    m_cabinetSprite.emplace(*m_cabinetTexture);
    m_cabinetSprite->setTextureRect(sf::IntRect({ 0, 0 }, { 128, 110 }));
    m_cabinetSprite->setScale({ 4.f, 4.f });

    layout(windowSize);
}

void ShopUI::layout(const sf::Vector2u& windowSize)
{
    if (!m_cabinetSprite) return;

    sf::Vector2f screenCenter(windowSize.x / 2.f, windowSize.y / 2.f);
    sf::FloatRect bounds = m_cabinetSprite->getGlobalBounds();

    m_cabinetSprite->setPosition({
        screenCenter.x - bounds.size.x / 2.f,
        screenCenter.y - bounds.size.y / 2.f
        });

    sf::Vector2f topLeft = m_cabinetSprite->getPosition() + sf::Vector2f(50.f, 150.f);
    sf::Vector2f cellSize(138.f, 138.f);

    int index = 0;
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 3; ++col) {
            sf::Vector2f pos = topLeft + sf::Vector2f(col * cellSize.x, row * cellSize.y);

            m_slots[index].area = sf::FloatRect(pos, cellSize);

            m_slots[index].bg.setSize(cellSize);
            m_slots[index].bg.setPosition(pos);
            m_slots[index].bg.setFillColor(sf::Color(0, 0, 0, 0));
            m_slots[index].bg.setOutlineThickness(2.f);
            m_slots[index].bg.setOutlineColor(sf::Color::Transparent); // later: highlight
            ++index;
        }
    }
}

void ShopUI::open() { m_open = true; }
void ShopUI::close() { m_open = false; }
bool ShopUI::isOpen() const { return m_open; }

void ShopUI::handleEvent(const sf::Event& event)
{
    if (!m_open) return;

    if (const auto* mouseButtonPressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos(
                static_cast<float>(mouseButtonPressed->position.x),
                static_cast<float>(mouseButtonPressed->position.y)
            );

            for (auto& slot : m_slots) {
                if (slot.area.contains(mousePos)) {
                    slot.bg.setOutlineColor(sf::Color::Yellow);
                }
            }
        }
    }
}

void ShopUI::update(float )
{
    if (!m_open) return;
}

void ShopUI::render(sf::RenderWindow& window)
{
    if (!m_open || !m_cabinetSprite) return;

    window.draw(m_overlay);
    window.draw(*m_cabinetSprite);

    for (auto& slot : m_slots)
        window.draw(slot.bg);

    Debug::DrawShopSlots(window, m_slots);
}