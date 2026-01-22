#ifndef HUB_H
#define HUB_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>
#include "Chunk.h"
#include "Headers/Player.h"
#include "ShopUI.h"
#include "Portal.h"


class Hub
{
public:
    Hub();
    void HandleResize(const sf::Vector2u& newSize);

    void Load(const sf::Texture& tileset, 
        std::vector<Chunk>& chunks, 
        player& player, float& chunkWidth, 
        const sf::Font& font, 
        const sf::Vector2u& windowSize);

    bool Update(float dt, const player& player);

    void Render(
        sf::RenderWindow& window,
        std::vector<Chunk>& chunks,
        player& player,
        const sf::Vector2f& cameraOffset,
        bool showDebugCollision,
        float PLAYER_HITBOX_WIDTH,
        float PLAYER_HITBOX_HEIGHT);

    bool IsShopOpen() const { return m_shopUI.isOpen(); }

    ShopUI& GetShopUI() { return m_shopUI; }

    sf::Texture m_portalTexture;
    Portal      m_portal;

private:
    bool m_startExpedition;
    bool m_eKeyPressed = false;
    // Interactable types
    enum class InteractableType
    {
        Shop,
        Portal
    };
    sf::Vector2u m_windowSize;
    // Shop UI state
    bool m_shopUIOpen = false;

    ShopUI m_shopUI;

    // Base interactable struct
    struct Interactable
    {
        sf::Sprite sprite;
        sf::Vector2f position;
        InteractableType type;
        float interactionRadius = 80.f;

        Interactable(const sf::Texture& texture, sf::Vector2f pos, InteractableType t)
            : sprite(texture), position(pos), type(t)
        {
            sprite.setPosition(pos);
        }
    };

    std::string getPromptFor(InteractableType t) const;

    // Textures
    sf::Texture m_shopTexture;       // Shop building (interactable)
    sf::Texture m_cabinetTexture;    // Cabinet UI (overlay when shop is open)

    // All interactables
    std::vector<Interactable> m_interactables;

    // Interaction prompt
    std::optional<sf::Text> m_promptText;
    int m_nearestInteractableIndex = -1;

    // Methods
    void LoadInteractables();
    void CheckProximity(const player& player);
    void HandleInteraction(InteractableType type);
    void OpenShop();
    void CloseShop();
    void RenderInteractables(sf::RenderWindow& window, const sf::Vector2f& cameraOffset);
    void RenderPrompt(sf::RenderWindow& window, const sf::Vector2f& cameraOffset);
};

#endif