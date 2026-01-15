#include "ShopUI.h"
#include "Debug.h"
#include <iostream>

ShopUI::ShopUI() = default;

void ShopUI::load(const sf::Texture& cabinetTex,
    const sf::Font& font,
    const sf::Vector2u& windowSize)
{
    m_cabinetTexture = &cabinetTex;
    m_font = &font;

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
    if (!m_cabinetSprite || !m_font) return;

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
            m_slots[index].bg.setOutlineColor(sf::Color::Transparent);

            // Setup text objects (construct optionals with font)
            m_slots[index].nameText.emplace(*m_font);
            m_slots[index].nameText->setCharacterSize(16);
            m_slots[index].nameText->setFillColor(sf::Color::White);

            m_slots[index].costText.emplace(*m_font);
            m_slots[index].costText->setCharacterSize(20);
            m_slots[index].costText->setFillColor(sf::Color::Yellow);

            ++index;
        }
    }
}

void ShopUI::generateShopItems()
{
    std::cout << "Generating shop items..." << std::endl;

    for (auto& slot : m_slots)
    {
        // Get random item from database
        Item item = m_itemDatabase.getRandomShopItem();
        slot.item = item;

        // Setup name text
        if (slot.nameText)
        {
            slot.nameText->setString(item.name);
            slot.nameText->setFillColor(getRarityColor(item.rarity));

            sf::FloatRect nameRect = slot.nameText->getLocalBounds();
            slot.nameText->setOrigin({
                nameRect.position.x + nameRect.size.x / 2.f,
                nameRect.position.y
                });
            slot.nameText->setPosition({
                slot.area.position.x + slot.area.size.x / 2.f,
                slot.area.position.y + 10.f
                });
        }

        // Setup cost text
        if (slot.costText)
        {
            slot.costText->setString(std::to_string(item.cost) + "g");

            sf::FloatRect costRect = slot.costText->getLocalBounds();
            slot.costText->setOrigin({
                costRect.position.x + costRect.size.x / 2.f,
                costRect.position.y
                });
            slot.costText->setPosition({
                slot.area.position.x + slot.area.size.x / 2.f,
                slot.area.position.y + slot.area.size.y - 30.f
                });
        }

        std::cout << "  Slot: " << item.name << " (" << item.cost << "g)" << std::endl;
    }
}

sf::Color ShopUI::getRarityColor(ItemRarity rarity) const
{
    switch (rarity)
    {
    case ItemRarity::Common:   return sf::Color(200, 200, 200); // Light gray
    case ItemRarity::Uncommon: return sf::Color(0, 255, 0);     // Green
    case ItemRarity::Rare:     return sf::Color(0, 150, 255);   // Blue
    case ItemRarity::Epic:     return sf::Color(200, 0, 255);   // Purple
    default:                   return sf::Color::White;
    }
}

void ShopUI::open()
{
    m_open = true;
    generateShopItems(); // Generate new items each time shop opens
}

void ShopUI::close()
{
    m_open = false;
}

bool ShopUI::isOpen() const
{
    return m_open;
}

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
                    // Highlight selected slot
                    slot.bg.setOutlineColor(sf::Color::Yellow);

                    // Print item info (later: actual purchase logic)
                    if (slot.item.has_value()) {
                        std::cout << "Clicked: " << slot.item->name
                            << " - Cost: " << slot.item->cost << "g" << std::endl;
                    }
                }
                else {
                    // Unhighlight other slots
                    slot.bg.setOutlineColor(sf::Color::Transparent);
                }
            }
        }
    }
}

void ShopUI::update(float)
{
    if (!m_open) return;
    // Could add hover effects, animations, etc.
}

void ShopUI::render(sf::RenderWindow& window)
{
    if (!m_open || !m_cabinetSprite) return;

    window.draw(m_overlay);
    window.draw(*m_cabinetSprite);

    // Draw slots and their items
    for (auto& slot : m_slots)
    {
        window.draw(slot.bg);

        if (slot.item.has_value())
        {
            if (slot.nameText)
                window.draw(*slot.nameText);
            if (slot.costText)
                window.draw(*slot.costText);
        }
    }

    // Optional: Keep debug visualization
    Debug::DrawShopSlots(window, m_slots);
}