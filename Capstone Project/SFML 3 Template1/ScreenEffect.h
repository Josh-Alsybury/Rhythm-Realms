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
        None,           // No effects
        Hub,            // Calm, subtle effects for hub area
        Expedition,     // Dynamic health/BPM-based effects for combat
        Menu,           // Decorative effects for menus
        Boss,           // Intense effects for boss fights
        LowHealth,      // Critical health warning effects
        Custom          // User-defined parameters
    };

    /// <summary>
    /// Individual effect types that can be combined
    /// </summary>
    enum class EffectType
    {
        Vignette            = 1 << 0,  // Edge vignette (expedition)
        HubLighting         = 1 << 1,  // Dark hub with player light (shader-based)
        ChromaticAberration = 1 << 2,  // Color separation (future)
        Distortion          = 1 << 3,  // Screen warping (future)
        Overlay             = 1 << 4,  // Color overlay (future)
        Scanlines           = 1 << 5   // CRT scanlines (future)
    };

    ScreenEffect();
    ~ScreenEffect() = default;

    /// <summary>
    /// Initialize the shaders and render targets
    /// </summary>
    /// <param name="windowSize">Size of the game window</param>
    /// <returns>True if initialization successful</returns>
    bool initialize(sf::Vector2u windowSize);


    void onResize(sf::Vector2u newSize);

    /// <summary>
    /// Set the current effect mode
    /// </summary>
    /// <param name="mode">Mode to switch to</param>
    void setMode(Mode mode);

    /// <summary>
    /// Enable/disable specific effects (bitwise)
    /// Example: enableEffects(EffectType::Vignette | EffectType::Scanlines)
    /// </summary>
    /// <param name="effects">Bitwise combination of effects</param>
    void enableEffects(int effects);

    /// <summary>
    /// Update effects for Expedition mode (health + BPM)
    /// </summary>
    /// <param name="healthRatio">Player health ratio (0.0 to 1.0)</param>
    /// <param name="bpmPulse">BPM pulse phase (0.0 to 1.0)</param>
    void updateExpedition(float healthRatio, float bpmPulse);

    /// <summary>
    /// Update effects for Hub mode (calm, static)
    /// </summary>
    /// <param name="deltaTime">Time since last update</param>
    void updateHub(float deltaTime);

    /// <summary>
    /// Update effects for Menu mode (decorative)
    /// </summary>
    /// <param name="deltaTime">Time since last update</param>
    void updateMenu(float deltaTime);

    /// <summary>
    /// Update effects for Boss mode (intense)
    /// </summary>
    /// <param name="bossHealthRatio">Boss health ratio (0.0 to 1.0)</param>
    /// <param name="playerHealthRatio">Player health ratio (0.0 to 1.0)</param>
    /// <param name="bpmPulse">BPM pulse phase (0.0 to 1.0)</param>
    void updateBoss(float bossHealthRatio, float playerHealthRatio, float bpmPulse);

    /// <summary>
    /// Set custom effect parameters
    /// </summary>
    /// <param name="color">RGB color (0-1 range)</param>
    /// <param name="intensity">Overall intensity (0-1)</param>
    /// <param name="pulse">Pulse amount (0-1)</param>
    void setCustom(sf::Vector3f color, float intensity, float pulse = 0.5f);

    /// <summary>
    /// Set vignette-specific parameters
    /// </summary>
    /// <param name="color">Edge color</param>
    /// <param name="intensity">How strong the effect is</param>
    /// <param name="softness">How soft the transition is (0=hard, 1=very soft)</param>
    void setVignetteParams(sf::Vector3f color, float intensity, float softness = 0.5f);

    /// <summary>
    /// Initialize hub lighting shader
    /// </summary>
    /// <param name="ambientDarkness">How dark the hub is (0-1, higher = darker)</param>
    bool initializeHubLighting(float ambientDarkness = 0.9f);

    /// <summary>
    /// Update player light position (for hub mode)
    /// </summary>
    /// <param name="playerScreenPosition">Current player screen position (after camera offset applied)</param>
    void updatePlayerLight(sf::Vector2f playerScreenPosition);

    /// <summary>
    /// Set hub light properties
    /// </summary>
    /// <param name="range">How far the light reaches (pixels)</param>
    /// <param name="color">Light color</param>
    void setHubLightParams(float range, sf::Color color = sf::Color(255, 220, 180));

    /// <summary>
    /// Render all active effects
    /// </summary>
    /// <param name="window">Window to render to</param>
    void render(sf::RenderWindow& window);

    /// <summary>
    /// Check if shaders are loaded and ready
    /// </summary>
    bool isReady() const { return m_shaderLoaded; }

    /// <summary>
    /// Get current mode
    /// </summary>
    Mode getMode() const { return m_currentMode; }

    /// <summary>
    /// Check if a specific effect is enabled
    /// </summary>
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
    int m_activeEffects;  // Bitwise flags

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
    
    // Fallback rendering (if shaders not supported)
    void renderFallback(sf::RenderWindow& window);
};

#endif // SCREEN_EFFECT_HPP
