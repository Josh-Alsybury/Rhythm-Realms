/// <summary>
/// ScreenEffect.cpp
/// Implementation of shader-based screen effects
/// </summary>
#include "ScreenEffect.h"
#include <iostream>
#include <cmath>

ScreenEffect::ScreenEffect()
    : m_shaderLoaded(false)
    , m_hubLightingLoaded(false)
    , m_currentMode(Mode::None)
    , m_activeEffects(0)
    , m_vignetteColor(1.0f, 1.0f, 1.0f)
    , m_vignetteIntensity(0.0f)
    , m_vignetteSoftness(0.5f)
    , m_vignettePulse(0.5f)
    , m_animationTime(0.0f)
    , m_playerScreenPos(0.f, 0.f)
    , m_hubLightRange(300.f)
    , m_hubLightColor(255, 220, 180)
    , m_hubAmbientDarkness(0.9f)
{
}

bool ScreenEffect::initialize(sf::Vector2u windowSize)
{
    // Check if shaders are supported
    if (!sf::Shader::isAvailable())
    {
        std::cout << "Shaders not supported on this system - using fallback" << std::endl;
        m_shaderLoaded = false;
        return false;
    }

    // Load the expedition vignette shader
    if (!m_vignetteShader.loadFromFile("ASSETS/Shaders/expedition_vignette.frag", sf::Shader::Type::Fragment))
    {
        std::cout << "Failed to load expedition vignette shader - using fallback" << std::endl;
        m_shaderLoaded = false;
    }
    else
    {
        std::cout << "Expedition vignette shader loaded!" << std::endl;
        m_shaderLoaded = true;
    }

    // Setup fullscreen quad
    m_fullscreenQuad.setSize(sf::Vector2f(static_cast<float>(windowSize.x), 
                                          static_cast<float>(windowSize.y)));
    m_fullscreenQuad.setPosition({0.0f, 0.0f});

    // Set resolution uniform (constant)
    if (m_shaderLoaded)
    {
        m_vignetteShader.setUniform("u_resolution", 
            sf::Vector2f(static_cast<float>(windowSize.x), 
                        static_cast<float>(windowSize.y)));
    }

    // Start with no effects
    setMode(Mode::None);

    return m_shaderLoaded;
}

void ScreenEffect::onResize(sf::Vector2u newSize)
{
    m_fullscreenQuad.setSize(sf::Vector2f(newSize));
    m_vignetteShader.setUniform("u_resolution", sf::Vector2f(newSize));

}

bool ScreenEffect::initializeHubLighting(float ambientDarkness)
{
    if (!sf::Shader::isAvailable())
    {
        std::cout << "Shaders not supported - hub lighting disabled" << std::endl;
        m_hubLightingLoaded = false;
        return false;
    }

    // Load the hub lighting shader
    if (!m_hubLightingShader.loadFromFile("ASSETS/Shaders/hub_lighting.frag", sf::Shader::Type::Fragment))
    {
        std::cout << "Failed to load hub lighting shader" << std::endl;
        m_hubLightingLoaded = false;
        return false;
    }

    std::cout << "Hub lighting shader loaded!" << std::endl;
    m_hubLightingLoaded = true;
    m_hubAmbientDarkness = std::clamp(ambientDarkness, 0.0f, 1.0f);

    // Set resolution uniform
    m_hubLightingShader.setUniform("u_ambientDarkness", m_hubAmbientDarkness);

    return true;
}

void ScreenEffect::setMode(Mode mode)
{
    m_currentMode = mode;

    // Set default parameters and enabled effects for each mode
    switch (mode)
    {
    case Mode::None:
        m_activeEffects = 0;
        m_vignetteIntensity = 0.0f;
        break;

    case Mode::Hub:
        m_activeEffects = static_cast<int>(EffectType::HubLighting);
        m_animationTime = 0.0f;
        break;

    case Mode::Expedition:
        m_activeEffects = static_cast<int>(EffectType::Vignette);
        m_vignetteColor = sf::Vector3f(0.0f, 1.0f, 0.0f); // Start green
        m_vignetteIntensity = 0.5f;
        m_vignetteSoftness = 0.5f;
        m_vignettePulse = 0.5f;
        break;

    case Mode::Menu:
        m_activeEffects = static_cast<int>(EffectType::Vignette);
        m_vignetteColor = sf::Vector3f(0.5f, 0.5f, 0.8f); // Purple-ish
        m_vignetteIntensity = 0.4f;
        m_vignetteSoftness = 0.7f;
        m_vignettePulse = 0.5f;
        break;

    case Mode::Boss:
        m_activeEffects = static_cast<int>(EffectType::Vignette);
        m_vignetteColor = sf::Vector3f(1.0f, 0.0f, 0.0f); // Red
        m_vignetteIntensity = 0.7f; // Intense
        m_vignetteSoftness = 0.3f; // Harder edges
        m_vignettePulse = 0.8f;
        break;

    case Mode::LowHealth:
        m_activeEffects = static_cast<int>(EffectType::Vignette);
        m_vignetteColor = sf::Vector3f(1.0f, 0.0f, 0.0f); // Red
        m_vignetteIntensity = 0.8f; // Very intense
        m_vignetteSoftness = 0.4f;
        m_vignettePulse = 0.9f;
        break;

    case Mode::Custom:
        // Will be set by setCustom()
        break;
    }
}

void ScreenEffect::enableEffects(int effects)
{
    m_activeEffects = effects;
}

void ScreenEffect::updateExpedition(float healthRatio, float bpmPulse)
{
    if (!isEffectEnabled(EffectType::Vignette))
        return;

    // Calculate health-based color
    m_vignetteColor = calculateHealthColor(healthRatio);

    // Intensity increases as health decreases
    m_vignetteIntensity = 0.3f + (1.0f - healthRatio) * 0.5f; // 0.3 to 0.8

    // Use BPM pulse directly
    m_vignettePulse = bpmPulse;

    // Update shader if loaded
    updateVignetteShader();
}

void ScreenEffect::updateHub(float deltaTime)
{
    if (!isEffectEnabled(EffectType::HubLighting))
        return;

    m_animationTime += deltaTime;
}

void ScreenEffect::updateMenu(float deltaTime)
{
    if (!isEffectEnabled(EffectType::Vignette))
        return;

    // Slow pulsing for menu
    m_animationTime += deltaTime;
    float pulse = std::sin(m_animationTime * 0.8f) * 0.5f + 0.5f;
    
    m_vignettePulse = 0.4f + pulse * 0.3f;

    updateVignetteShader();
}

void ScreenEffect::updateBoss(float bossHealthRatio, float playerHealthRatio, float bpmPulse)
{
    if (!isEffectEnabled(EffectType::Vignette))
        return;

    // Intense red that gets stronger as boss health drops
    m_vignetteColor = sf::Vector3f(1.0f, 0.2f * bossHealthRatio, 0.0f);

    // Very intense, modulated by player health
    m_vignetteIntensity = 0.6f + (1.0f - playerHealthRatio) * 0.3f;

    // Strong pulse with BPM
    m_vignettePulse = 0.5f + bpmPulse * 0.5f;

    updateVignetteShader();
}

void ScreenEffect::setCustom(sf::Vector3f color, float intensity, float pulse)
{
    m_currentMode = Mode::Custom;
    m_vignetteColor = color;
    m_vignetteIntensity = intensity;
    m_vignettePulse = pulse;
    
    // Enable vignette for custom mode
    m_activeEffects = static_cast<int>(EffectType::Vignette);

    updateVignetteShader();
}

void ScreenEffect::setVignetteParams(sf::Vector3f color, float intensity, float softness)
{
    m_vignetteColor = color;
    m_vignetteIntensity = intensity;
    m_vignetteSoftness = std::clamp(softness, 0.0f, 1.0f);

    updateVignetteShader();
}

void ScreenEffect::updatePlayerLight(sf::Vector2f playerScreenPosition)
{
    m_playerScreenPos = playerScreenPosition;
}

void ScreenEffect::setHubLightParams(float range, sf::Color color)
{
    m_hubLightRange = range;
    m_hubLightColor = color;
}

void ScreenEffect::render(sf::RenderWindow& window)
{
    // Don't render if no effects are active
    if (m_currentMode == Mode::None || m_activeEffects == 0)
        return;

    // Render hub lighting
    if (isEffectEnabled(EffectType::HubLighting) && m_hubLightingLoaded)
    {
        // Update shader uniforms
        m_hubLightingShader.setUniform("u_playerPosition", m_playerScreenPos);
        m_hubLightingShader.setUniform("u_lightRange", m_hubLightRange);
        
        sf::Vector3f lightColorNormalized(
            m_hubLightColor.r / 255.f,
            m_hubLightColor.g / 255.f,
            m_hubLightColor.b / 255.f
        );
        m_hubLightingShader.setUniform("u_lightColor", lightColorNormalized);
        m_hubLightingShader.setUniform("u_ambientDarkness", m_hubAmbientDarkness);

        window.draw(m_fullscreenQuad, &m_hubLightingShader);
    }

    // Render vignette (expedition mode)
    if (isEffectEnabled(EffectType::Vignette))
    {
        if (m_shaderLoaded)
        {
            window.draw(m_fullscreenQuad, &m_vignetteShader);
        }
        else
        {
            renderFallback(window);
        }
    }
}

void ScreenEffect::updateVignetteShader()
{
    if (!m_shaderLoaded)
        return;

    // Update shader uniforms
    m_vignetteShader.setUniform("u_healthColor", m_vignetteColor);
    m_vignetteShader.setUniform("u_healthRatio", 1.0f - m_vignetteIntensity); // Invert for shader
    m_vignetteShader.setUniform("u_bpmPulse", m_vignettePulse);
    m_vignetteShader.setUniform("u_intensity", m_vignetteIntensity);
}

sf::Vector3f ScreenEffect::calculateHealthColor(float healthRatio)
{
    // Clamp health ratio
    healthRatio = std::clamp(healthRatio, 0.0f, 1.0f);

    sf::Vector3f color;

    if (healthRatio > 0.5f)
    {
        // Green to yellow (high health)
        float t = (healthRatio - 0.5f) / 0.5f; // 0 to 1
        color.x = 1.0f - t;  // R: 1 -> 0
        color.y = 1.0f;      // G: 1
        color.z = 0.0f;      // B: 0
    }
    else
    {
        // Yellow to red (low health)
        float t = healthRatio / 0.5f; // 0 to 1
        color.x = 1.0f;      // R: 1
        color.y = t;         // G: 0 -> 1
        color.z = 0.0f;      // B: 0
    }

    return color;
}

void ScreenEffect::renderFallback(sf::RenderWindow& window)
{
    // Fallback rendering if shaders not supported
    // Simple rectangle-based vignette
    auto size = window.getSize();
    
    // Convert color from 0-1 to 0-255
    sf::Color color(
        static_cast<std::uint8_t>(m_vignetteColor.x * 255),
        static_cast<std::uint8_t>(m_vignetteColor.y * 255),
        static_cast<std::uint8_t>(m_vignetteColor.z * 255),
        static_cast<std::uint8_t>(m_vignetteIntensity * m_vignettePulse * 128)
    );

    const int NUM_LAYERS = 6;
    for (int i = 0; i < NUM_LAYERS; ++i)
    {
        float t = static_cast<float>(i) / (NUM_LAYERS - 1);
        float thickness = 15.0f + t * 30.0f;
        
        std::uint8_t alpha = static_cast<std::uint8_t>(
            m_vignetteIntensity * m_vignettePulse * 200.0f * (1.0f - t)
        );
        
        sf::Color layerColor = color;
        layerColor.a = alpha;
        
        // Top
        sf::RectangleShape top({static_cast<float>(size.x), thickness});
        top.setPosition({0.0f, t * 50.0f});
        top.setFillColor(layerColor);
        window.draw(top);
        
        // Bottom
        sf::RectangleShape bottom({static_cast<float>(size.x), thickness});
        bottom.setPosition({0.0f, size.y - t * 50.0f - thickness});
        bottom.setFillColor(layerColor);
        window.draw(bottom);
        
        // Left
        sf::RectangleShape left({thickness, static_cast<float>(size.y)});
        left.setPosition({t * 50.0f, 0.0f});
        left.setFillColor(layerColor);
        window.draw(left);
        
        // Right
        sf::RectangleShape right({thickness, static_cast<float>(size.y)});
        right.setPosition({size.x - t * 50.0f - thickness, 0.0f});
        right.setFillColor(layerColor);
        window.draw(right);
    }
}
