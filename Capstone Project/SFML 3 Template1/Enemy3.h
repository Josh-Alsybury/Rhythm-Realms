#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath> 
#include "EnemyTextures.h"

class Enemy3
{
public:
    // --- States ---
    enum class EnemyState {
        Idle,
        Walking,      // Changed from Running
        Attack1,      // First attack
        Attack2       // Second attack
    };

    // Animation type enum safe from vector reallocation
    enum class AnimationType {
        Idle,
        Walking,      // Changed from Running
        Attack1,      // First attack
        Attack2       // Second attack
    };

    // --- Animation Info ---
    struct Animation {
        sf::Texture* texture = nullptr;
        int frameCount = 0;
        int frameWidth = 0;
        int frameHeight = 0;
    };

    // --- Sprite and Movement ---
    std::unique_ptr<sf::Sprite> sprite;
    sf::Vector2f pos;
    sf::Vector2f velocity;
    bool facingRight = false;
    float speed = 70.f;
    float maxSpeed = 150.f;
    float friction = 500.f;
    float gravity = 50.f;
    float groundLevel = 750.f;
    bool isOnGround = false;

    // --- Animations (stored as members) ---
    Animation idleAnim;
    Animation walkAnim;      // Changed from runAnim
    Animation attack1Anim;   // First attack
    Animation attack2Anim;   // Second attack

    // --- Animation Timing ---
    int m_frameNow{ 0 };
    float m_frameCount{ 0.0f };
    float m_framePlus{ 0.2f };

    // --- State ---
    EnemyState state = EnemyState::Idle;

    // --- Attack tracking ---
    int m_lastAttackUsed = 1;  // Track which attack was used last (1 or 2)

    // --- Fuzzy logic setters ---
    void setSpeed(float s) { speed = s; }
    void setAttackCooldown(float c) { attackCooldownTime = c; }

    // --- Detection Circles ---
    sf::CircleShape detectionRadius;
    sf::CircleShape attackRadius;
    float detectionRange = 450.f;
    float attackRange = 120.f;

    // --- Attack System ---
    sf::CircleShape attackHitbox;
    float attackHitboxRadius = 50.f;
    bool canDamagePlayer = false;
    bool hasDealtDamage = false;
    float attackCooldown = 0.f;
    float attackCooldownTime = 1.0f;

    bool m_isStunned = false;
    float m_stunTimer = 0.f;
    const float STUN_DURATION = 2.0f;
    std::vector<sf::CircleShape> m_stunStars;
    bool m_stunStarsInitialized = false;
    float m_knockbackVelocity = 0.f;
    const float KNOCKBACK_FRICTION = 800.f;
    float m_flashTimer = 0.f;
    const float FLASH_DURATION = 0.1f;

    // --- Health System ---
    int health;
    int MAX_HEALTH = 8;
    std::vector<sf::RectangleShape> healthBar;

    // --- Constructor / Setup ---
    Enemy3();
    void SetupEnemy3();  // Changed from SetupEnemy1
    void Reset();

    // --- Update & Animation ---
    void Update(float dt, sf::Vector2f playerPos);
    void AnimateEnemy(float dt);

    // --- State Changes ---
    void SetState(EnemyState newState);

    // --- AI ---
    float DistanceToPlayer(sf::Vector2f playerPos);
    void AIBehavior(sf::Vector2f playerPos, float dt);

    // --- Health / damage ---
    void UpdateHealthBar();
    void TakeDamage(int amount);

private:
    // Get animation safely (prevents vector reallocation issues)
    Animation* GetCurrentAnimation();

    // Current animation TYPE (not pointer!)
    AnimationType currentAnimType;
};