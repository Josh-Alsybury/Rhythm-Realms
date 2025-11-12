#include "Headers/DynamicBackground.h"

bool DynamicBackground::loadtheme(const std::string& folderPath)
{
    m_layers.clear();

    for (int i = 1; i <= 3; ++i)
    {
        std::string filePath = folderPath + "/layer_" + std::to_string(i) + ".png"; // loads background paths
        if (!std::filesystem::exists(filePath))
        {
            filePath = folderPath + "/" + std::to_string(i) + ".png";
        }
        float parallax = 0.3f * (4 - i);
        int layerIndex = i - 1;
        Background layer(filePath, parallax, layerIndex);
        m_layers.push_back(std::move(layer));
    }

    m_currentTheme = folderPath;
    return false;
}

void DynamicBackground::update(const sf::Vector2f& cameraOffset)
{
    for (auto& layer : m_layers)
    {
        layer.setOffset(cameraOffset);
    }

    if (m_isTransitioning)
    {
        m_transitionTimer += 0.016f; // ~60 FPS
        float progress = m_transitionTimer / m_transitionDuration;

        if (progress >= 1.0f)
        {
            // Transition complete
            m_layers = std::move(m_newLayers);
            m_newLayers.clear();
            m_currentTheme = m_nextTheme;
            m_isTransitioning = false;
            m_transitionTimer = 0.0f;
            for (auto& layer : m_layers)
            {
                layer.setOpacity(1.0f);
            }
        }
        else
        {
            for (int i = 2; i >= 0; --i)
            {
                int reverseIndex = 2 - i;
                float layerStartTime = reverseIndex * 0.4f;
                float layerProgress = (progress - layerStartTime) / 0.6f;
                layerProgress = std::clamp(layerProgress, 0.0f, 1.0f);

                float opacity = 1.0f - layerProgress; // Fade out
                m_layers[i].setOpacity(opacity);
            }

            for (int i = 2; i >= 0; --i)
            {
                int reverseIndex = 2 - i;
                float layerStartTime = reverseIndex * 0.4f;
                float layerProgress = (progress - layerStartTime) / 0.6f;
                layerProgress = std::clamp(layerProgress, 0.0f, 1.0f);

                float opacity = layerProgress; // Fade in
                m_newLayers[i].setOpacity(opacity);
                m_newLayers[i].setOffset(cameraOffset);
            }
        }
    }
}

void DynamicBackground::render(sf::RenderWindow& window)
{
    for (int i = m_layers.size() - 1; i >= 0; --i)
    {
        m_layers[i].render(window); // inverse drawing to get parrallax
    }
    if (m_isTransitioning)
    {
        for (int i = m_newLayers.size() - 1; i >= 0; --i)
        {
            m_newLayers[i].render(window);
        }
    }
}

void DynamicBackground::transitionTo(const std::string& newFolderPath)
{
    if (m_currentTheme == newFolderPath || m_isTransitioning)
        return;
    m_newLayers.clear();
    for (int i = 1; i <= 3; ++i)
    {
        std::string filePath = newFolderPath + "/layer_" + std::to_string(i) + ".png";
        if (!std::filesystem::exists(filePath))
        {
            filePath = newFolderPath + "/" + std::to_string(i) + ".png";
        }
        float parallax = 0.3f * (4 - i);
        int layerIndex = i - 1;  
        Background layer(filePath, parallax, layerIndex);
        layer.setOpacity(0.0f);
        m_newLayers.push_back(std::move(layer));
    }
    m_nextTheme = newFolderPath;
    m_isTransitioning = true;
    m_transitionTimer = 0.0f;
}
