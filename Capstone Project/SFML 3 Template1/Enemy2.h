#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <cmath>

class Enemy2
{
public:
    enum class EnemyState {
        Idle,
        Running,
        Attacking,
        Defending
    };

    struct Animation {
        sf::Texture* texture = nullptr;
        int frameCount = 0;
        int frameWidth = 0;
        int frameHeight = 0;
    };

    // Constructor & Setup
    Enemy2();
    void SetupEnemy2();
    void Reset();

    // Update & Render
    void Update(float dt, sf::Vector2f playerPos);
    void AnimateEnemy(float dt);

    // Combat
    void TakeDamage(int amount);
    void UpdateHealthBar();
    float DistanceToPlayer(sf::Vector2f playerPos);

    // Public members (same structure as Enemy1)
    sf::Vector2f pos;
    sf::Vector2f velocity;
    std::unique_ptr<sf::Sprite> sprite;
    int health;
    static constexpr int MAX_HEALTH = 2;  // Archers are weaker

    // Attack system
    bool canDamagePlayer;
    bool hasDealtDamage;
    sf::CircleShape attackHitbox;
    float attackHitboxRadius;

    // Health bar
    std::vector<sf::RectangleShape> healthBar;

    // State
    EnemyState state;
    bool facingRight;

    // Debug visuals
    sf::CircleShape detectionRadius;
    sf::CircleShape attackRadius;

private:
    // AI & State
    void AIBehavior(sf::Vector2f playerPos, float dt);
    void SetState(EnemyState newState);

    // Textures
    sf::Texture idleTexture;
    sf::Texture runTexture;
    sf::Texture attackTexture;
    sf::Texture defendTexture;

    // Animations
    Animation idleAnim;
    Animation runAnim;
    Animation attackAnim;
    Animation defendAnim;
    Animation* currentAnim;

    // Animation state
    float m_frameCount;
    int m_frameNow;
    float m_framePlus;

    // Archer-specific combat stats
    float speed;                 // 150 (slower than melee)
    float detectionRange;        // 600 (longer range)
    float attackRange;           // 400 (ranged)
    float preferredDistance;     // 350 (keeps distance)
    float attackCooldown;
    float attackCooldownTime;    // 2.5s (slower attacks)
};