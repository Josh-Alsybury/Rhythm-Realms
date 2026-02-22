#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <cmath>
#include "EnemyTextures.h"

class Enemy2
{
public:
    // State enum 
    enum class ArcherState
    {
        Idle,
        Running,
        Attacking,
        Defending
    };

    // Animation type enum 
    enum class AnimationType
    {
        Idle,
        Running,
        Attacking,
        Defending
    };

    // Animation struct 
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
    static constexpr int MAX_HEALTH = 6;  // Archers are weaker
    std::vector<sf::RectangleShape> healthBar;

    // --- State ---
    ArcherState state;
    bool isInitialised = false;

    // --- Animations
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
    float attackCooldownTime; // 1.5s (slower attack)
    float preferredDistance;  // 250 (keep distance)
    bool m_isStunned = false;
    float m_stunTimer = 0.f;
    const float STUN_DURATION = 0.3f;
    std::vector<sf::CircleShape> m_stunStars;
    bool m_stunStarsInitialized = false;

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

    // HELPER: Get animation safely 
    Animation* GetCurrentAnimation();

    // Current animation TYPE
    AnimationType currentAnimType;
};