#include "Hub.h"
#include "Headers/DynamicBackground.h"
#include "Debug.h"
#include <iostream>
#include <cmath>

Hub::Hub()
    : m_startExpedition(false)
    , m_nearestInteractableIndex(-1)
    , m_promptText(std::nullopt)
    , m_shopUIOpen(false)
    , m_eKeyPressed(false)
{
}

void Hub::Load(const sf::Texture& tileset,
    std::vector<Chunk>& chunks,
    player& player,
    float& chunkWidth,
    const sf::Font& font,
    const sf::Vector2u& windowSize)
{
    std::cout << "=== LOADING HUB ===" << std::endl;

    auto hubChunks = DynamicBackground::GetChunkPaths(GameTheme::Hub);
    chunks.clear();
    chunks.resize(hubChunks.size());

    std::string hubTilesetPath = DynamicBackground::GetTilesetPath(GameTheme::Hub);
    float xPos = 0.f;

    for (int i = 0; i < hubChunks.size(); ++i)
    {
        if (!chunks[i].load(hubChunks[i], tileset, 32, hubTilesetPath))
        {
            std::cout << "Failed to load hub chunk " << i << std::endl;
            continue;
        }
        chunks[i].setPosition({ xPos, 190.f });
        xPos += chunks[i].getWidth();
    }

    chunkWidth = chunks[0].getWidth();
    player.pos = { 320.f, 650.f };

    // SFML 3: Initialize text with font
    m_promptText.emplace(font);
    m_promptText->setCharacterSize(24);
    m_promptText->setFillColor(sf::Color::Yellow);
    m_promptText->setString("[E] Shop");
    sf::FloatRect bounds = m_promptText->getLocalBounds();
    m_promptText->setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y
    });

    // Load interactables
    LoadInteractables();

    m_shopUI.load(m_cabinetTexture, font, windowSize);

    std::cout << "=== HUB LOADED ===" << std::endl;
}

std::string Hub::getPromptFor(InteractableType t) const
{
    switch (t)
    {
    case InteractableType::Shop:   return "[E] Shop";
    case InteractableType::Portal: return "[E] Enter Portal";
    }
    return "";
}


void Hub::LoadInteractables()
{
    // Load shop building texture
    if (!m_shopTexture.loadFromFile("ASSETS/Shop/Shop_Static_84x60.png"))
    {
        std::cerr << "Failed to load shop texture!" << std::endl;
        return;
    }

    // Load cabinet UI texture (this is the overlay)
    if (!m_cabinetTexture.loadFromFile("ASSETS/Shop/Cabinets.png"))
    {
        std::cerr << "Failed to load cabinet texture!" << std::endl;
        return;
    }

    if (!m_portalTexture.loadFromFile("ASSETS/Portal/Green Portal Sprite Sheet.png"))
    {
        std::cerr << "Failed to load Portal texture!" << std::endl;
        return;
    }

    m_portal.load(m_portalTexture, { 800.f, 750.f }); // world position

    m_interactables.clear();

    std::cout << "Portal texture size: "
        << m_portalTexture.getSize().x << " x "
        << m_portalTexture.getSize().y << "\n";

    sf::Vector2f portalPos = { 800.f, 720.f }; // same as m_portal.load
    m_portal.load(m_portalTexture, portalPos); // ensure worldPos matches

    m_interactables.emplace_back(m_portalTexture, portalPos, InteractableType::Portal);
    auto& portalInteract = m_interactables.back();
    portalInteract.interactionRadius = 80.f;
    portalInteract.position = portalPos;

    sf::Vector2f shopBasePos = { 1100.f, 810.f }; 

    m_interactables.emplace_back(m_shopTexture, shopBasePos, InteractableType::Shop);
    auto& shop = m_interactables.back();

    shop.sprite.setTextureRect(sf::IntRect({ 0, 0 }, { 84, 60 }));
    shop.sprite.setScale({ 2.5f, 2.5f });

    sf::FloatRect bounds = shop.sprite.getLocalBounds();

    // bottom center origin for drawing
    shop.sprite.setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y
    });

    shop.sprite.setPosition(shopBasePos);

    float circleOffsetY = -bounds.size.y * 0.35f; 
    shop.position = shopBasePos + sf::Vector2f(0.f, circleOffsetY);
    shop.interactionRadius = 75.f;
}

bool Hub::Update(float dt, const player& player)
{
    // Check if E key is currently pressed
    bool eKeyCurrentlyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E);

    // If shop UI is open
    if (m_shopUIOpen)
    {
        // Detect E key press (not hold) - close shop
        if (eKeyCurrentlyPressed && !m_eKeyPressed)
        {
            CloseShop();
        }

        // ESC always closes
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
        {
            CloseShop();
        }

        m_eKeyPressed = eKeyCurrentlyPressed;
        return false;
    }

    // Check proximity to interactables
    CheckProximity(player);

    // Handle E key interaction (detect press, not hold)
    if (eKeyCurrentlyPressed && !m_eKeyPressed)
    {
        if (m_nearestInteractableIndex >= 0)
        {
            InteractableType type = m_interactables[m_nearestInteractableIndex].type;
            HandleInteraction(type);
        }
    }

    m_portal.update(dt);

    // Update key state
    m_eKeyPressed = eKeyCurrentlyPressed;

    if (m_startExpedition)
    {
        m_startExpedition = false; // reset for next time
        return true;
    }

    return false;
}

void Hub::CheckProximity(const player& player)
{
    m_nearestInteractableIndex = -1;
    float nearestDistance = 999999.f;

    for (int i = 0; i < m_interactables.size(); ++i)
    {
        auto& interactable = m_interactables[i];

        float dx = player.pos.x - interactable.position.x;
        float dy = player.pos.y - interactable.position.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance < interactable.interactionRadius && distance < nearestDistance)
        {
            nearestDistance = distance;
            m_nearestInteractableIndex = i;
        }
    }
}

void Hub::HandleInteraction(InteractableType type)
{
    switch (type)
    {
    case InteractableType::Shop:
        OpenShop();
        break;

    case InteractableType::Portal:
        m_startExpedition = true;
        break;
    }
   
}

void Hub::OpenShop()
{
    m_shopUI.open();
    m_shopUIOpen = true;
    std::cout << "Shop opened!" << std::endl;
}

void Hub::CloseShop()
{
    m_shopUI.close();
    m_shopUIOpen = false;
    std::cout << "Shop closed!" << std::endl;
}

void Hub::Render(
    sf::RenderWindow& window,
    std::vector<Chunk>& chunks,
    player& player,
    const sf::Vector2f& cameraOffset,
    bool showDebugCollision,
    float PLAYER_HITBOX_WIDTH,
    float PLAYER_HITBOX_HEIGHT)
{
    // Render hub chunks
    for (auto& chunk : chunks)
    {
        chunk.draw(window, cameraOffset);
    }

    m_portal.render(window, cameraOffset);

    // Render interactables (shop building, etc.)
    RenderInteractables(window, cameraOffset);

    // Debug collision
    if (showDebugCollision)
    {
        Debug::DrawChunkCollision(window, chunks, cameraOffset);
        Debug::DrawPlayerCollision(window, player, cameraOffset,
            PLAYER_HITBOX_WIDTH, PLAYER_HITBOX_HEIGHT);

        // Draw interaction radius circles
        for (auto& interactable : m_interactables)
        {
            sf::CircleShape radiusCircle(interactable.interactionRadius);
            radiusCircle.setOrigin({ interactable.interactionRadius, interactable.interactionRadius });
            radiusCircle.setPosition(interactable.position - cameraOffset);
            radiusCircle.setFillColor(sf::Color(0, 255, 255, 50));
            radiusCircle.setOutlineColor(sf::Color::Cyan);
            radiusCircle.setOutlineThickness(2.f);
            window.draw(radiusCircle);
        }
    }

    // Render player
    window.draw(*player.sprite);

    // Render player UI
    for (auto& bar : player.HealBar)
        window.draw(bar);
    for (int i = 0; i < player.HealsCount; i++)
        window.draw(player.HealSphere[i]);

    // Render interaction prompt (if not in shop AND near something)
    if (!m_shopUIOpen && m_nearestInteractableIndex >= 0)
    {
        RenderPrompt(window, cameraOffset);
    }

    if (m_shopUI.isOpen())
    {
        sf::View oldView = window.getView();
        window.setView(window.getDefaultView());

        m_shopUI.render(window);

        window.setView(oldView);
    }

}

void Hub::RenderInteractables(sf::RenderWindow& window, const sf::Vector2f& cameraOffset)
{
    for (auto& interactable : m_interactables)
    {
        // Skip drawing portal sprite, only draw shop
        if (interactable.type == InteractableType::Portal)
            continue;

        sf::Sprite sprite = interactable.sprite;
        sprite.setPosition(interactable.position - cameraOffset);
        window.draw(sprite);
    }
}

void Hub::RenderPrompt(sf::RenderWindow& window, const sf::Vector2f& cameraOffset)
{
    if (m_nearestInteractableIndex < 0 || !m_promptText) return;

    auto& interactable = m_interactables[m_nearestInteractableIndex];

    // Set text based on type
    m_promptText->setString(getPromptFor(interactable.type));

    // Recenter origin because width can change
    sf::FloatRect bounds = m_promptText->getLocalBounds();
    m_promptText->setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y
        });

    sf::Vector2f worldPos = interactable.position;
    worldPos.y -= 90.f;

    m_promptText->setPosition(worldPos - cameraOffset);
    window.draw(*m_promptText);
}

