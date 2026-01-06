#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath> 
#include "EnemyTextures.h"

class Enemy1
{
public:
    // --- States ---
    enum class EnemyState {
        Idle,
        Running,
        Attacking,
        Defending
    };

    // Animation type enum - safe from vector reallocation
    enum class AnimationType {
        Idle,
        Running,
        Attacking,
        Defending
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
    Animation runAnim;
    Animation attackAnim;
    Animation defendAnim;

    // --- Animation Timing ---
    int m_frameNow{ 0 };
    float m_frameCount{ 0.0f };
    float m_framePlus{ 0.2f };

    // --- State ---
    EnemyState state = EnemyState::Idle;

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

    // --- Health System ---
    int health = 2;
    static constexpr int MAX_HEALTH = 2;
    std::vector<sf::RectangleShape> healthBar;

    // --- Constructor / Setup ---
    Enemy1();
    void SetupEnemy1();
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
    // HELPER: Get animation safely (prevents vector reallocation issues)
    Animation* GetCurrentAnimation();

    // Current animation TYPE (not pointer!)
    AnimationType currentAnimType;
};