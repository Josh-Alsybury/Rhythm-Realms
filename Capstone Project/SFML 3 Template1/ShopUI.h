#pragma once
#include <SFML/Graphics.hpp>
#include <array>

struct ShopSlotUI {
    sf::FloatRect area;
    sf::RectangleShape bg;
};

class ShopUI {
public:
    ShopUI();

    void load(const sf::Texture& cabinetTex,
        const sf::Font& font,
        const sf::Vector2u& windowSize);

    void open();
    void close();
    bool isOpen() const;

    void handleEvent(const sf::Event& event);
    void update(float dt);
    void render(sf::RenderWindow& window);

private:
    void layout(const sf::Vector2u& windowSize);

    bool m_open = false;

    const sf::Texture* m_cabinetTexture = nullptr;
    std::optional<sf::Sprite> m_cabinetSprite;

    sf::RectangleShape m_overlay;
    std::array<ShopSlotUI, 6> m_slots;
};
