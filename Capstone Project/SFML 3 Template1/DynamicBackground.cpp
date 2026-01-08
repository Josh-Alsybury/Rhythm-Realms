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
    return true;
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
    std::cout << "Transitioning BG to: " << newFolderPath << std::endl;
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

GameTheme DynamicBackground::GetThemeFromBPM(float bpm)
{
    if (bpm > 150.0f)
        return GameTheme::Factory;
    else if (bpm >= 90.0f && bpm <= 150.0f)
        return GameTheme::Forest;
    else // < 90
        return GameTheme::Medieval;
}

std::string DynamicBackground::GetBackgroundPath(GameTheme theme)
{
    switch (theme)
    {
    case GameTheme::Forest:
        return "ASSETS/IMAGES/Autumn Forest 2D Pixel Art/Background";
    case GameTheme::Medieval:
        return "ASSETS/IMAGES/Background";
    case GameTheme::Factory:
        return "ASSETS/IMAGES/Factory/Background";
    default:
        return "ASSETS/IMAGES/Autumn Forest 2D Pixel Art/Background";
    }
}

std::string DynamicBackground::GetTilesetTexturePath(GameTheme theme)
{
    switch (theme)
    {
    case GameTheme::Forest:
        return "ASSETS/Tileset/Tileset.png";
    case GameTheme::Medieval:
        return "ASSETS/Tileset/TilesetMedival.png";
    case GameTheme::Factory:
        return "ASSETS/Tileset/TilesetFactory.png";
    case GameTheme::Hub:
        return "ASSETS/Tileset/TilesetHub.png";
    default:
        return "ASSETS/Tileset/Tileset.png";
    }
}

std::string DynamicBackground::GetTilesetPath(GameTheme theme)
{
    switch (theme)
    {
    case GameTheme::Forest:
        return "ASSETS/Tiles/Forest.tsj";
    case GameTheme::Medieval:
        return "ASSETS/Tiles/Medival.tsj";
    case GameTheme::Factory:
        return "ASSETS/Tiles/Factory.tsj";
    case GameTheme::Hub:
        return "ASSETS/Tiles/Hub.tsj";
    default:
        return "ASSETS/Tiles/Forest.tsj";
    }
}

std::vector<std::string> DynamicBackground::GetChunkPaths(GameTheme theme)
{
    switch (theme)
    {
    case GameTheme::Forest:
        return {
            "ASSETS/CHUNKS/Chunk1(Forest).tmj",
            "ASSETS/CHUNKS/Chunk2(Forest).tmj",
            "ASSETS/CHUNKS/Chunk3(Forest).tmj",
            "ASSETS/CHUNKS/Chunk4(Forest).tmj"
        };

    case GameTheme::Medieval:
        return {
            "ASSETS/CHUNKS/Chunk1(Medival).tmj",
            "ASSETS/CHUNKS/Chunk2(Medival).tmj",
            "ASSETS/CHUNKS/Chunk3(Medival).tmj",
            "ASSETS/CHUNKS/Chunk4(Medival).tmj"
        };

    case GameTheme::Factory:
        return {
            "ASSETS/CHUNKS/Chunk1(Factory).tmj",
            "ASSETS/CHUNKS/Chunk2(Factory).tmj",
            "ASSETS/CHUNKS/Chunk3(Factory).tmj",
            "ASSETS/CHUNKS/Chunk4(Factory).tmj"
        };

    case GameTheme::Hub:
        return {
            "ASSETS/CHUNKS/Chunk1(hub).tmj",
            "ASSETS/CHUNKS/Chunk2(hub).tmj",
            "ASSETS/CHUNKS/Chunk3(hub).tmj"
        };

    default:
        return GetChunkPaths(GameTheme::Forest);
    }
}