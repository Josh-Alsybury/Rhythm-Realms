#include "SkillTree.h"
#include <iostream>

SkillTree::SkillTree() : m_skillPoints(0)
{
    m_skills = {
        // Survival branch
        { "Extra Heal I",   "+1 Heal Charge",          2, false, Skill::Branch::Survivability, 0, Skill::Type::EXTRA_HEAL,          0, 1 },
        { "Extra Heal II",  "+1 Heal Charge",          4, false, Skill::Branch::Survivability, 1, Skill::Type::EXTRA_HEAL,          0, 1 },
        { "Extra Health",   "+20 Max HP",              3, false, Skill::Branch::Survivability, 2, Skill::Type::EXTRA_HEALTH,        0, 1 },
        { "Overheal",       "Kills overheal above max", 4, false, Skill::Branch::Survivability, 3, Skill::Type::OVERHEAL,           0, 3 },

        // Offense branch
        { "Multiplier",     "Raise base combo floor",  2, false, Skill::Branch::Offense,       0, Skill::Type::MULTIPLIER_BOOST,   0, 3 },
        { "Attack Speed",   "Faster combo window",     3, false, Skill::Branch::Offense,       1, Skill::Type::ATTACK_SPEED,       0, 1 },

        // Mobilty branch
        { "Perfect Parry",  "Parry: speed or power",   3, false, Skill::Branch::Mobility,      0, Skill::Type::PERFECT_BLOCK_BONUS, 0, 2 },

        // Rhythm branch BPM locked
        { "Low BPM Dmg",    "+20% taken dmg at low BPM",    2, false, Skill::Branch::Rhythm,        0, Skill::Type::BPM_DAMAGE_BOOST,   0, 3 },
        { "High BPM Def",   "-15% given dmg at high BPM",   2, false, Skill::Branch::Rhythm,        1, Skill::Type::BPM_DEFENCE_BOOST,  0, 3 },
    };

    // Build branch index lists
    m_branchIndices.resize(4);
    for (int i = 0; i < static_cast<int>(m_skills.size()); ++i)
    {
        auto b = static_cast<int>(m_skills[i].branch);
        m_branchIndices[b].push_back(i);
    }

    // Layout branches radiating around centre
    sf::Vector2f center(500.f, 400.f);
    int branchCount = static_cast<int>(m_branchIndices.size());

    for (int b = 0; b < branchCount; ++b)
    {
        float angle = (3.1415926f * 2.f) * (static_cast<float>(b) / branchCount);
        sf::Vector2f branchDir(std::cos(angle), std::sin(angle));

        const auto& indices = m_branchIndices[b];
        for (int t = 0; t < static_cast<int>(indices.size()); ++t)
        {
            int skillIndex = indices[t];
            float dist = 80.f + t * 90.f;
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

    // Fully maxed
    if (s.maxStacks > 1 && s.stackCount >= s.maxStacks) return false;
    if (s.maxStacks == 1 && s.unlocked) return false;

    // Cant afford
    if (m_skillPoints < s.cost) return false;

    // BPM locks applies to tier 0 too
    if (s.type == Skill::Type::BPM_DAMAGE_BOOST && m_currentBPM >= 100.f)
        return false;
    if (s.type == Skill::Type::BPM_DEFENCE_BOOST && m_currentBPM < 130.f)
        return false;

    // Tier 0 is always unlockable if affordable
    if (s.tier == 0) return true;

    // Check previous in same branch is unlocked or has at least 1 stack
    auto branch = s.branch;
    for (int i = 0; i < static_cast<int>(m_skills.size()); ++i)
    {
        if (m_skills[i].branch == branch && m_skills[i].tier == s.tier - 1)
        {
            if (m_skills[i].unlocked || m_skills[i].stackCount > 0)
                return true;
        }
    }

    return false;
}

void SkillTree::UnlockSkill(int skillIndex)
{
    if (!CanUnlock(skillIndex)) return;

    auto& s = m_skills[skillIndex];
    m_skillPoints -= s.cost;

    if (s.maxStacks > 1)
    {
        s.stackCount++;
        // Cost scales up each stack
        s.cost = static_cast<int>(s.cost * 1.8f);

        if (s.stackCount >= s.maxStacks)
            s.unlocked = true;
    }
    else
    {
        s.unlocked = true;
    }

    // Node colour by branch
    sf::Color c;
    switch (s.branch)
    {
    case Skill::Branch::Survivability: c = sf::Color(50, 200, 50);  break;
    case Skill::Branch::Offense:       c = sf::Color(220, 80, 80);  break;
    case Skill::Branch::Mobility:      c = sf::Color(80, 180, 220); break;
    case Skill::Branch::Rhythm:        c = sf::Color(200, 200, 80); break;
    }

    // Dim slightly if not  maxed 
    if (s.maxStacks > 1 && !s.unlocked)
        c = sf::Color(c.r / 2, c.g / 2, c.b / 2);

    m_nodes[skillIndex].setFillColor(c);
    std::cout << "Unlocked: " << s.name << " (stack " << s.stackCount << "/" << s.maxStacks << ")\n";
}

bool SkillTree::IsSkillUnlocked(int skillIndex)
{
    if (skillIndex < 0 || skillIndex >= static_cast<int>(m_skills.size()))
        return false;
    return m_skills[skillIndex].unlocked || m_skills[skillIndex].stackCount > 0;
}

int SkillTree::GetStackCount(int skillIndex)
{
    if (skillIndex < 0 || skillIndex >= static_cast<int>(m_skills.size()))
        return 0;
    return m_skills[skillIndex].stackCount;
}

void SkillTree::Draw(sf::RenderWindow& window)
{
    // Draw conecting lines
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
                sf::Vertex{a,    sf::Color(150, 150, 150)},
                sf::Vertex{bPos, sf::Color(150, 150, 150)}
            };
            window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
        }
    }

    // Draw nodes an stack numbers
    for (size_t i = 0; i < m_nodes.size(); ++i)
    {
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

        // Stack number on node
        if (m_skills[i].maxStacks > 1)
        {
            sf::Text stackText{ m_font };
            stackText.setCharacterSize(16);
            stackText.setFillColor(sf::Color::White);
            stackText.setString(std::to_string(m_skills[i].stackCount)
                + "/" + std::to_string(m_skills[i].maxStacks));
            sf::FloatRect bounds = stackText.getLocalBounds();
            stackText.setPosition(
                m_nodes[i].getPosition() - sf::Vector2f(bounds.size.x / 2.f, bounds.size.y / 2.f)
            );
            window.draw(stackText);
        }
    }

    // Skill points
    sf::Text pointsText{ m_font };
    pointsText.setString("Skill Points: " + std::to_string(m_skillPoints));
    pointsText.setCharacterSize(24);
    pointsText.setPosition(sf::Vector2f(50.f, 50.f));
    window.draw(pointsText);

    // BPM indicator for rhythm branch
    sf::Text bpmText{ m_font };
    bpmText.setCharacterSize(18);
    bpmText.setFillColor(sf::Color(200, 200, 80));
    bpmText.setString("Current BPM: " + std::to_string((int)m_currentBPM));
    bpmText.setPosition(sf::Vector2f(50.f, 80.f));
    window.draw(bpmText);

    sf::Text instructionsText{ m_font };
    instructionsText.setString("Press T to close | Click nodes to unlock | Hover for info");
    instructionsText.setCharacterSize(18);
    instructionsText.setFillColor(sf::Color(200, 200, 200));
    instructionsText.setPosition(sf::Vector2f(50.f, 750.f));
    window.draw(instructionsText);

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
            UnlockSkill(static_cast<int>(i));
            return;
        }
    }
}

void SkillTree::DrawTooltip(sf::RenderWindow& window, int skillIndex, sf::Vector2f mousePos)
{
    const Skill& skill = m_skills[skillIndex];

    sf::RectangleShape tooltipBox;
    tooltipBox.setSize(sf::Vector2f(260.f, 120.f));
    tooltipBox.setFillColor(sf::Color(40, 40, 40, 230));
    tooltipBox.setOutlineThickness(2.f);
    tooltipBox.setOutlineColor(sf::Color::White);

    sf::Vector2f tooltipPos = mousePos + sf::Vector2f(20.f, 20.f);
    if (tooltipPos.x + 260.f > window.getSize().x) tooltipPos.x = mousePos.x - 280.f;
    if (tooltipPos.y + 120.f > window.getSize().y) tooltipPos.y = mousePos.y - 140.f;

    tooltipBox.setPosition(tooltipPos);
    window.draw(tooltipBox);

    sf::Text nameText{ m_font };
    nameText.setString(skill.name);
    nameText.setCharacterSize(20);
    nameText.setFillColor(skill.unlocked ? sf::Color::Green : sf::Color::Yellow);
    nameText.setPosition(tooltipPos + sf::Vector2f(10.f, 8.f));
    window.draw(nameText);

    sf::Text descText{ m_font };
    descText.setString(skill.description);
    descText.setCharacterSize(16);
    descText.setFillColor(sf::Color::White);
    descText.setPosition(tooltipPos + sf::Vector2f(10.f, 36.f));
    window.draw(descText);

    // Stack info
    std::string costStr;
    if (skill.unlocked && skill.maxStacks == 1)
    {
        costStr = "UNLOCKED";
    }
    else if (skill.maxStacks > 1)
    {
        costStr = "Stacks: " + std::to_string(skill.stackCount) + "/" + std::to_string(skill.maxStacks);
        costStr += "  Next: " + std::to_string(skill.cost) + " SP";
    }
    else
    {
        costStr = "Cost: " + std::to_string(skill.cost) + " SP";
    }

    sf::Text costText{ m_font };
    costText.setString(costStr);
    costText.setCharacterSize(16);
    costText.setFillColor(CanUnlock(skillIndex) ? sf::Color::White : sf::Color::Red);
    costText.setPosition(tooltipPos + sf::Vector2f(10.f, 60.f));
    window.draw(costText);

    // BPM lock 
    if (skill.type == Skill::Type::BPM_DAMAGE_BOOST && m_currentBPM > 100.f)
    {
        sf::Text lockText{ m_font };
        lockText.setString("Requires low BPM < 100");
        lockText.setCharacterSize(14);
        lockText.setFillColor(sf::Color(255, 100, 100));
        lockText.setPosition(tooltipPos + sf::Vector2f(10.f, 85.f));
        window.draw(lockText);
    }
    if (skill.type == Skill::Type::BPM_DEFENCE_BOOST && m_currentBPM < 145.f)
    {
        sf::Text lockText{ m_font };
        lockText.setString("Requires high BPM > 145");
        lockText.setCharacterSize(14);
        lockText.setFillColor(sf::Color(255, 100, 100));
        lockText.setPosition(tooltipPos + sf::Vector2f(10.f, 85.f));
        window.draw(lockText);
    }
}

void SkillTree::UpdateHover(sf::Vector2f mousePos)
{
    m_hoveredNodeIndex = -1;
    for (size_t i = 0; i < m_nodes.size(); ++i)
    {
        if (m_nodes[i].getGlobalBounds().contains(mousePos))
        {
            m_hoveredNodeIndex = static_cast<int>(i);
            return;
        }
    }
}


SkillTree::SkillModifiers SkillTree::GetModifiers()
{
    SkillModifiers mods;

    for (auto& s : m_skills)
    {
        if (!s.unlocked && s.stackCount == 0) continue;

        switch (s.type)
        {
        case Skill::Type::EXTRA_HEAL:
            mods.bonusHeals++;
            break;
        case Skill::Type::EXTRA_HEALTH:
            mods.bonusMaxHP += 20;
            break;
        case Skill::Type::OVERHEAL:
            mods.overhealCap = 1.0f + (s.stackCount * 0.1f);
            break;
        case Skill::Type::MULTIPLIER_BOOST:
            mods.baseComboMultiplier = 1.0f + (s.stackCount * 0.5f);
            break;
        case Skill::Type::ATTACK_SPEED:
            mods.hasAttackSpeed = true;
            break;
        case Skill::Type::PERFECT_BLOCK_BONUS:
            mods.hasPerfectParry = true;
            mods.perfectParryStacks = s.stackCount;
            break;
        case Skill::Type::BPM_DAMAGE_BOOST:
            mods.bpmDamageBoost = 1.0f + (s.stackCount * 0.2f);
            break;
        case Skill::Type::BPM_DEFENCE_BOOST:
            mods.bpmDefenceBoost = 1.0f + (s.stackCount * 0.15f);
            break;
        }
    }
    return mods;
}
