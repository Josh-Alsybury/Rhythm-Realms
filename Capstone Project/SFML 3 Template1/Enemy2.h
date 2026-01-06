#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <cmath>
#include "EnemyTextures.h"

class Enemy2
{
public:
    // State enum (renamed from EnemyState to ArcherState for clarity)
    enum class ArcherState
    {
        Idle,
        Running,
        Attacking,
        Defending
    };

    // Animation type enum - safe from vector reallocation
    enum class AnimationType
    {
        Idle,
        Running,
        Attacking,
        Defending
    };

    // Animation struct - MUST be declared before any methods use it
    struct Animation {
        sf::Texture* texture = nullptr;
        int frameCount = 0;
        int frameWidth = 0;
        int frameHeight = 0;
    };

    // --- Sprite and Movement ---
    sf::Vector2f pos;
    sf::Vector2f velocity;
    std::unique_ptr<sf::Sprite> sprite;
    bool facingRight;
    float speed;

    // --- Health System ---
    int health;
    static constexpr int MAX_HEALTH = 2;  // Archers are weaker
    std::vector<sf::RectangleShape> healthBar;

    // --- State ---
    ArcherState state;
    bool isInitialised = false;

    // --- Animations (stored as members, accessed safely via GetCurrentAnimation()) ---
    Animation idleAnim;
    Animation runAnim;
    Animation attackAnim;
    Animation defendAnim;

    // --- Animation Timing ---
    int m_frameNow{ 0 };
    float m_frameCount{ 0.0f };
    float m_framePlus{ 0.05f };

    // --- Detection Circles ---
    sf::CircleShape detectionRadius;
    sf::CircleShape attackRadius;
    float detectionRange;     // 600 (longer range)
    float attackRange;        // 400 (ranged)

    // --- Attack System ---
    sf::CircleShape attackHitbox;
    float attackHitboxRadius;
    bool canDamagePlayer;
    bool hasDealtDamage;
    float attackCooldown;
    float attackCooldownTime; // 2.5s (slower attacks)
    float preferredDistance;  // 350 (keeps distance)

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

private:
    // AI & State
    void AIBehavior(sf::Vector2f playerPos, float dt);
    void SetState(ArcherState newState);

    // HELPER: Get animation safely (prevents vector reallocation issues)
    Animation* GetCurrentAnimation();

    // Current animation TYPE (not pointer!)
    AnimationType currentAnimType;
};