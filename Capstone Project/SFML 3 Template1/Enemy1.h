#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath> 

#pragma once
class Enemy1
{
public:
    // --- Textures ---
    sf::Texture idleTexture;
    sf::Texture runTexture;
    sf::Texture attackTexture;
    sf::Texture defendTexture;

    // --- Sprite and Movement ---
    std::unique_ptr<sf::Sprite> sprite;
    sf::Vector2f pos;
    sf::Vector2f velocity;
    bool facingRight = false;

    float speed = 150.f;
    float maxSpeed = 300.f;
    float friction = 500.f;
    float gravity = 50.f;
    float groundLevel =750.f;

    // --- Animation Info ---
    struct Animation {
        sf::Texture* texture;
        int frameCount;
        int frameWidth;
        int frameHeight;
    };

    Animation idleAnim;
    Animation runAnim;
    Animation attackAnim;
    Animation defendAnim;
    Animation* currentAnim = nullptr;

    // --- Animation Timing ---
    int m_frameNow{ 0 };
    float m_frameCount{ 0.0f };
    float m_framePlus{ 0.2f };

    // --- States ---
    enum class EnemyState {
        Idle,
        Running,
        Attacking,
        Defending
    };

    EnemyState state = EnemyState::Idle;

    // --- Detection Circles ---
    sf::CircleShape detectionRadius;  // large "alert" 
    sf::CircleShape attackRadius;     // smaller "close" 
    float detectionRange = 450.f;     // how far the enemy senses the player
    float attackRange = 120.f;       // how close to start attacking

    sf::CircleShape attackHitbox;
    float attackHitboxRadius = 50.f;
    bool canDamagePlayer = false;
    bool hasDealtDamage = false;

    // Attack cooldown
    float attackCooldown = 0.f;
    float attackCooldownTime = 1.0f;

    // Health system
    int health = 2;
    static constexpr int MAX_HEALTH = 2;
    std::vector<sf::RectangleShape> healthBar;


    // --- Constructor / Setup ---
    Enemy1();
    void SetupEnemy1();

    // --- Update & Animation ---
    void Update(float dt,sf::Vector2f playerPos);
    void AnimateEnemy(float dt);

    // --- State Changes ---
    void SetState(EnemyState newState);


    // --- Realistic ai distance reader ---
    float DistanceToPlayer(sf::Vector2f playerPos);

    void AIBehavior(sf::Vector2f playerPos, float dt);

    // --- Health / damage ---
    void UpdateHealthBar();
    void TakeDamage(int amount);
};


