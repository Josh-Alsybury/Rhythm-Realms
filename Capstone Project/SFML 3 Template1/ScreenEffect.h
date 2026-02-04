/// <summary>
/// ScreenEffect.h
/// Handles shader-based screen effects (vignette, hub lighting, etc.)
/// Supports multiple modes for different game states
/// </summary>
#ifndef SCREEN_EFFECT_HPP
#define SCREEN_EFFECT_HPP

#include <SFML/Graphics.hpp>
#include <memory>

class ScreenEffect
{
public:
    /// <summary>
    /// Visual modes for different game states
    /// </summary>
    enum class Mode
    {
        None,           
        Hub,           
        Expedition,     
        Menu,           
        Boss,           
        LowHealth,      
        Custom          
    };

    /// <summary>
    /// Individual effect types that can be combined
    /// </summary>
    enum class EffectType
    {
        Vignette            = 1 << 0, 
        HubLighting         = 1 << 1,  
        ChromaticAberration = 1 << 2,  
        Distortion          = 1 << 3,  
        Overlay             = 1 << 4,  
        Scanlines           = 1 << 5   
    };

    ScreenEffect();
    ~ScreenEffect() = default;

    bool initialize(sf::Vector2u windowSize);
    void onResize(sf::Vector2u newSize);
    void setMode(Mode mode);
    void enableEffects(int effects);

    void updateExpedition(float healthRatio, float bpmPulse);
    void updateHub(float deltaTime);
    void updateMenu(float deltaTime);
    void updateBoss(float bossHealthRatio, float playerHealthRatio, float bpmPulse);
    void setCustom(sf::Vector3f color, float intensity, float pulse = 0.5f);

    void setVignetteParams(sf::Vector3f color, float intensity, float softness = 0.5f);
    bool initializeHubLighting(float ambientDarkness = 0.9f);
    void updatePlayerLight(sf::Vector2f playerScreenPosition);
    void setHubLightParams(float range, sf::Color color = sf::Color(255, 220, 180));

    void render(sf::RenderWindow& window);
    bool isReady() const { return m_shaderLoaded; }
    Mode getMode() const { return m_currentMode; }
    bool isEffectEnabled(EffectType effect) const { return (m_activeEffects & static_cast<int>(effect)) != 0; }

private:
    // Shaders
    sf::Shader m_vignetteShader;     // Expedition vignette
    sf::Shader m_hubLightingShader;  // Hub lighting
    sf::RectangleShape m_fullscreenQuad;
    bool m_shaderLoaded;
    bool m_hubLightingLoaded;

    // Hub lighting parameters
    sf::Vector2f m_playerScreenPos;
    float m_hubLightRange;
    sf::Color m_hubLightColor;
    float m_hubAmbientDarkness;

    // State
    Mode m_currentMode;
    int m_activeEffects; 

    // Vignette parameters
    sf::Vector3f m_vignetteColor;
    float m_vignetteIntensity;
    float m_vignetteSoftness;
    float m_vignettePulse;

    // Animation
    float m_animationTime;

    // Helper functions
    sf::Vector3f calculateHealthColor(float healthRatio);
    void updateVignetteShader();
    
    // Fallback rendering
    void renderFallback(sf::RenderWindow& window);
};

#endif // SCREEN_EFFECT_HPP
