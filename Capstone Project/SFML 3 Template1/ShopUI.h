#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <optional>
#include "Item.h"
#include "ItemDatabase.h"

struct ShopSlotUI {
    sf::FloatRect area;
    sf::RectangleShape bg;
    std::optional<Item> item;  // The item in this slot
    std::optional<sf::Text> nameText;  // Make optional for SFML 3 compatibility
    std::optional<sf::Text> costText;  // Make optional for SFML 3 compatibility
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
    void generateShopItems();  // Populate slots with random items
    sf::Color getRarityColor(ItemRarity rarity) const;

    bool m_open = false;
    const sf::Texture* m_cabinetTexture = nullptr;
    const sf::Font* m_font = nullptr;
    std::optional<sf::Sprite> m_cabinetSprite;
    sf::RectangleShape m_overlay;
    std::array<ShopSlotUI, 6> m_slots;
    ItemDatabase m_itemDatabase;
};