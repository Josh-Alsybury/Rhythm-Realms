#include "SkillTree.h"
#include <iostream>

SkillTree::SkillTree() : m_skillPoints(0)
{

    m_skills = {
        // Survivability branch
        { "Extra Heal I", "+1 Heal Charge", 1, false, Skill::Branch::Survivability, 0, Skill::Type::EXTRA_HEAL },
        { "Extra Heal II", "+1 Heal Charge", 2, false, Skill::Branch::Survivability, 1, Skill::Type::EXTRA_HEAL },
        { "Extra Heart I", "+1 Max HP", 2, false, Skill::Branch::Survivability, 2, Skill::Type::EXTRA_HEALTH },
        { "Heal on Kill", "Small heal when killing enemies", 3, false, Skill::Branch::Survivability, 3, Skill::Type::HEAL_ON_KILL },

        // Offense branch
        { "Attack Up I", "+10% damage", 1, false, Skill::Branch::Offense, 0, Skill::Type::CRIT_CHANCE },
        { "Attack Up II", "+10% damage", 2, false, Skill::Branch::Offense, 1, Skill::Type::CRIT_CHANCE },
        { "Special Attack", "Unlocks special finisher", 4, false, Skill::Branch::Offense, 2, Skill::Type::SPECIAL_ATTACK },
        { "Attack Speed", "Faster combo window", 3, false, Skill::Branch::Offense, 3, Skill::Type::ATTACK_SPEED },

        // Mobility branch
        { "Dash", "Quick dash move", 2, false, Skill::Branch::Mobility, 0, Skill::Type::DASH },
        { "Dash I-frames", "Dash grants brief invulnerability", 3, false, Skill::Branch::Mobility, 1, Skill::Type::DASH },
        { "Double Dash", "2 dash charges", 4, false, Skill::Branch::Mobility, 2, Skill::Type::DASH },

        // Rhythm branch (BPM synergy)
        { "Perfect Guard", "Perfect blocks restore stamina", 2, false, Skill::Branch::Rhythm, 0, Skill::Type::PERFECT_BLOCK_BONUS },
        { "Beat Fury", "+damage at high BPM", 3, false, Skill::Branch::Rhythm, 1, Skill::Type::BPM_DAMAGE_BOOST },
        { "Beat Focus", "+defence at low BPM", 3, false, Skill::Branch::Rhythm, 2, Skill::Type::BPM_DEFENCE_BOOST }
    };

    // Build branch index lists
    m_branchIndices.resize(4);
    for (int i = 0; i < static_cast<int>(m_skills.size()); ++i)
    {
        auto b = static_cast<int>(m_skills[i].branch);
        m_branchIndices[b].push_back(i);
    }

    // Layout: 4 branches radiating around centre (like your reference)
    sf::Vector2f center(500.f, 400.f);
    float branchRadius = 180.f;
    int branchCount = static_cast<int>(m_branchIndices.size());

    for (int b = 0; b < branchCount; ++b)
    {
        float angle = (3.1415926f * 2.f) * (static_cast<float>(b) / branchCount);
        sf::Vector2f branchDir(std::cos(angle), std::sin(angle));

        const auto& indices = m_branchIndices[b];
        for (int t = 0; t < static_cast<int>(indices.size()); ++t)
        {
            int skillIndex = indices[t];
            float dist = 80.f + t * 70.f; // move outwards per tier
            sf::Vector2f pos = center + branchDir * dist;

            sf::CircleShape node(30.f);
            node.setOrigin({ 30.f, 30.f });
            node.setPosition(pos);
            node.setFillColor(sf::Color(100, 100, 100));
            node.setOutlineThickness(3.f);
            node.setOutlineColor(sf::Color::White);

            m_nodes.push_back(node);
        }
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
    if (skillIndex < 0 || skillIndex >= static_cast<int>(m_skills.size()))
        return false;

    Skill& s = m_skills[skillIndex];
    if (s.unlocked || m_skillPoints < s.cost)
        return false;

    // Find all skills in this branch and sort by tier
    auto branch = s.branch;
    std::vector<int> branchSkills;
    for (int i = 0; i < static_cast<int>(m_skills.size()); ++i)
        if (m_skills[i].branch == branch)
            branchSkills.push_back(i);

    // Starting node (tier 0) is always unlockable if you can afford it
    if (s.tier == 0) return true;

    // Must have at least one skill in this branch with tier < s.tier unlocked
    bool predecessorUnlocked = false;
    for (int idx : branchSkills)
    {
        if (m_skills[idx].tier == s.tier - 1 && m_skills[idx].unlocked)
        {
            predecessorUnlocked = true;
            break;
        }
    }

    return predecessorUnlocked;
}


void SkillTree::UnlockSkill(int skillIndex)
{
    if (!CanUnlock(skillIndex))
        return;

    auto& s = m_skills[skillIndex];
    s.unlocked = true;
    m_skillPoints -= s.cost;

    sf::Color c;
    switch (s.branch)
    {
    case Skill::Branch::Survivability: c = sf::Color(50, 200, 50); break;
    case Skill::Branch::Offense:       c = sf::Color(220, 80, 80); break;
    case Skill::Branch::Mobility:      c = sf::Color(80, 180, 220); break;
    case Skill::Branch::Rhythm:        c = sf::Color(200, 200, 80); break;
    }
    m_nodes[skillIndex].setFillColor(c);

    std::cout << "Unlocked: " << s.name << "\n";
}


bool SkillTree::IsSkillUnlocked(int skillIndex)
{
    return m_skills[skillIndex].unlocked;
}

void SkillTree::Draw(sf::RenderWindow& window)
{
    // Draw connecting lines
    for (int b = 0; b < static_cast<int>(m_branchIndices.size()); ++b)
    {
        const auto& indices = m_branchIndices[b];
        for (int i = 0; i + 1 < static_cast<int>(indices.size()); ++i)
        {
            int idxA = indices[i];
            int idxB = indices[i + 1];
            sf::Vector2f a = m_nodes[idxA].getPosition();
            sf::Vector2f bPos = m_nodes[idxB].getPosition();

            std::array<sf::Vertex, 2> line = {
                sf::Vertex{a, sf::Color(150,150,150)},
                sf::Vertex{bPos, sf::Color(150,150,150)}
            };
            window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
        }
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
