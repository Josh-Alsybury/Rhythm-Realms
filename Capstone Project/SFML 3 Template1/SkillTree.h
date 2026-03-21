#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
class SkillTree
{
public:
    struct Skill
    {
        std::string name;
        std::string description;
        int cost = 1;
        bool unlocked = false;

        enum class Branch {
            Survivability,
            Offense,
            Mobility,
            Rhythm
        } branch;

        int tier = 0;          // 0 starting node in that branch

        enum class Type {
            EXTRA_HEAL,
            EXTRA_HEALTH,
            OVERHEAL,
            MULTIPLIER_BOOST,
            ATTACK_SPEED,
            PERFECT_BLOCK_BONUS,
            BPM_DAMAGE_BOOST,
            BPM_DEFENCE_BOOST
        } type;
        int stackCount = 0;  
        int maxStacks = 1; //non stackable

        
    };
    struct SkillModifiers
    {
        int bonusHeals = 0;
        int bonusMaxHP = 0;
        float overhealCap = 1.0f;
        float baseComboMultiplier = 1.0f;
        bool hasAttackSpeed = false;
        bool hasPerfectParry = false;
        int perfectParryStacks = 0;
        float bpmDamageBoost = 1.0f;
        float bpmDefenceBoost = 1.0f;
    };

    SkillModifiers GetModifiers();

    std::vector<std::vector<int>> m_branchIndices;

    SkillTree();

    // Core functions
    void AddSkillPoint();                    // Gain point from defeating enemy
    bool CanUnlock(int skillIndex);          // Check if affordable
    void UnlockSkill(int skillIndex);        // Purchase skill
    bool IsSkillUnlocked(int skillIndex);    // Check if player has it
    int GetStackCount(int skillIndex);

    // UI
    void Draw(sf::RenderWindow& window);
    void HandleClick(sf::Vector2f mousePos);
    void UpdateHover(sf::Vector2f mousePos);

    int GetSkillPoints() { return m_skillPoints; }

    void SetCurrentBPM(float bpm) { m_currentBPM = bpm; }
    float m_currentBPM = 120.f;


private:
    std::vector<Skill> m_skills;
    int m_skillPoints;

    // UI elements
    std::vector<sf::CircleShape> m_nodes;
    sf::Font m_font;
    int m_hoveredNodeIndex = -1;  // -1 = no hover
    void DrawTooltip(sf::RenderWindow& window, int skillIndex, sf::Vector2f mousePo0s);
};

