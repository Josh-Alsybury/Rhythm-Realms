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
		int cost;
		bool unlocked;

        enum class Type {
            EXTRA_HEAL,         // +1 heal charge
            EXTRA_HEALTH,       // +1 max HP
            DASH,               // Unlock dash move
            SPECIAL_ATTACK      // Unlock special
        };
        Type type;

	};

    SkillTree();

    // Core functions
    void AddSkillPoint();                    // Gain point from defeating enemy
    bool CanUnlock(int skillIndex);          // Check if affordable
    void UnlockSkill(int skillIndex);        // Purchase skill
    bool IsSkillUnlocked(int skillIndex);    // Check if player has it

    // UI
    void Draw(sf::RenderWindow& window);
    void HandleClick(sf::Vector2f mousePos);
    void UpdateHover(sf::Vector2f mousePos);

    int GetSkillPoints() { return m_skillPoints; }

private:
    std::vector<Skill> m_skills;
    int m_skillPoints;

    // UI elements
    std::vector<sf::CircleShape> m_nodes;
    sf::Font m_font;
    int m_hoveredNodeIndex = -1;  // -1 = no hover
    void DrawTooltip(sf::RenderWindow& window, int skillIndex, sf::Vector2f mousePo0s);
};

