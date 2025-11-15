#include "SkillTree.h"
#include <iostream>

SkillTree::SkillTree() : m_skillPoints(0)
{
    // Define your skill tree 
    m_skills = {
        {"Extra Heal", "+1 Heal Charge", 1, false, Skill::Type::EXTRA_HEAL},
        {"Extra Heart", "+1 Max HP", 2, false, Skill::Type::EXTRA_HEALTH},
        {"Dash", "Quick dash move", 4, false, Skill::Type::DASH},
        {"Special Attack", "Powerful finisher", 5, false, Skill::Type::SPECIAL_ATTACK}
    };

    // Create visual nodes
    for (size_t i = 0; i < m_skills.size(); ++i)
    {
        sf::CircleShape node(30.f);
        node.setPosition(sf::Vector2f(100.f + i * 100.f, 400.f));
        node.setFillColor(sf::Color(100, 100, 100));  // Gray = locked
        node.setOutlineThickness(3.f);
        node.setOutlineColor(sf::Color::White);
        m_nodes.push_back(node);
    }

    if (!m_font.openFromFile("ASSETS/FONTS/Jersey20-Regular.ttf"))
        std::cout << "Failed to load font!\n";
}

void SkillTree::AddSkillPoint()
{
    m_skillPoints++;
    std::cout << "Skill point gained! Total: " << m_skillPoints << std::endl;
}

bool SkillTree::CanUnlock(int skillIndex)
{
    if (skillIndex < 0 || skillIndex >= m_skills.size())
        return false;

    // Check if already unlocked
    if (m_skills[skillIndex].unlocked)
        return false;

    // Check if can afford
    if (m_skillPoints < m_skills[skillIndex].cost)
        return false;

    // Check if previous skill unlocked 
    if (skillIndex > 0 && !m_skills[skillIndex - 1].unlocked)
        return false;

    return true;
}

void SkillTree::UnlockSkill(int skillIndex)
{
    if (!CanUnlock(skillIndex))
        return;

    m_skills[skillIndex].unlocked = true;
    m_skillPoints -= m_skills[skillIndex].cost;
    m_nodes[skillIndex].setFillColor(sf::Color::Green);  // Green = unlocked

    std::cout << "Unlocked: " << m_skills[skillIndex].name << std::endl;
}

bool SkillTree::IsSkillUnlocked(int skillIndex)
{
    return m_skills[skillIndex].unlocked;
}

void SkillTree::Draw(sf::RenderWindow& window)
{
    // Draw connecting lines
    for (size_t i = 0; i < m_nodes.size() - 1; ++i)
    {
        sf::Vector2f start = m_nodes[i].getPosition() + sf::Vector2f(30.f, 30.f);
        sf::Vector2f end = m_nodes[i + 1].getPosition() + sf::Vector2f(30.f, 30.f);

        std::array<sf::Vertex, 2> line = {
            sf::Vertex{start},
            sf::Vertex{end}
        };

        window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
    }

    for (size_t i = 0; i < m_nodes.size(); ++i)
    {
        // Highlight hovered node
        if (static_cast<int>(i) == m_hoveredNodeIndex)
        {
            m_nodes[i].setOutlineThickness(5.f);
            m_nodes[i].setOutlineColor(sf::Color::Yellow);
        }
        else
        {
            m_nodes[i].setOutlineThickness(3.f);
            m_nodes[i].setOutlineColor(sf::Color::White);
        }

        window.draw(m_nodes[i]);
    }

    // Draw nodes
    for (auto& node : m_nodes)
        window.draw(node);

    // Draw skill points text
    sf::Text pointsText{ m_font };
    pointsText.setFont(m_font);
    pointsText.setString("Skill Points: " + std::to_string(m_skillPoints));
    pointsText.setCharacterSize(24);
    pointsText.setPosition(sf::Vector2f(50.f, 50.f));
    window.draw(pointsText);

    sf::Text instructionsText{m_font};
    instructionsText.setFont(m_font);
    instructionsText.setString("Press T to close | Click nodes to unlock | Hover for info");
    instructionsText.setCharacterSize(18);
    instructionsText.setFillColor(sf::Color(200, 200, 200));
    instructionsText.setPosition(sf::Vector2f(50.f, 750.f));
    window.draw(instructionsText);

    // Draw tooltip if hovering over a node
    if (m_hoveredNodeIndex >= 0)
    {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);
        DrawTooltip(window, m_hoveredNodeIndex, mousePos);
    }
}

void SkillTree::HandleClick(sf::Vector2f mousePos)
{
    for (size_t i = 0; i < m_nodes.size(); ++i)
    {
        if (m_nodes[i].getGlobalBounds().contains(mousePos))
        {
            UnlockSkill(i);
            return;
        }
    }
}

void SkillTree::DrawTooltip(sf::RenderWindow& window, int skillIndex, sf::Vector2f mousePo0s)
{
    const Skill& skill = m_skills[skillIndex];

    // Create tooltip background
    sf::RectangleShape tooltipBox;
    tooltipBox.setSize(sf::Vector2f(250.f, 100.f));
    tooltipBox.setFillColor(sf::Color(40, 40, 40, 230)); 
    tooltipBox.setOutlineThickness(2.f);
    tooltipBox.setOutlineColor(sf::Color::White);

    // Position tooltip near mouse (
    sf::Vector2f tooltipPos = mousePo0s + sf::Vector2f(20.f, 20.f);

    // Keep tooltip on screen
    if (tooltipPos.x + 250.f > window.getSize().x)
        tooltipPos.x = mousePo0s.x - 270.f;
    if (tooltipPos.y + 100.f > window.getSize().y)
        tooltipPos.y = mousePo0s.y - 120.f;

    tooltipBox.setPosition(tooltipPos);
    window.draw(tooltipBox);

    // Skill name (title)
    sf::Text nameText{ m_font };
    nameText.setFont(m_font);
    nameText.setString(skill.name);
    nameText.setCharacterSize(22);
    nameText.setFillColor(skill.unlocked ? sf::Color::Green : sf::Color::Yellow);
    nameText.setPosition(tooltipPos + sf::Vector2f(10.f, 10.f));
    window.draw(nameText);

    // Description
    sf::Text descText{ m_font };
    descText.setFont(m_font);
    descText.setString(skill.description);
    descText.setCharacterSize(18);
    descText.setFillColor(sf::Color::White);
    descText.setPosition(tooltipPos + sf::Vector2f(10.f, 40.f));
    window.draw(descText);

    // Cost (or "UNLOCED")
    sf::Text costText{ m_font };
    costText.setFont(m_font);
    if (skill.unlocked)
    {
        costText.setString("UNLOCKED");
        costText.setFillColor(sf::Color::Green);
    }
    else
    {
        costText.setString("Cost: " + std::to_string(skill.cost) + " SP");
        costText.setFillColor(CanUnlock(skillIndex) ? sf::Color::White : sf::Color::Red);
    }
    costText.setCharacterSize(18);
    costText.setPosition(tooltipPos + sf::Vector2f(10.f, 68.f));
    window.draw(costText);
}

void SkillTree::UpdateHover(sf::Vector2f mousePos)
{
    m_hoveredNodeIndex = -1;  // Reset hover state

    for (size_t i = 0; i < m_nodes.size(); ++i)
    {
        if (m_nodes[i].getGlobalBounds().contains(mousePos))
        {
            m_hoveredNodeIndex = static_cast<int>(i);
            return;  // Found hovered node, stop checking
        }
    }
}
