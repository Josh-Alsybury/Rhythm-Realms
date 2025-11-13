#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#pragma once
class player
{
public:
    sf::Texture idleTexture;
    sf::Texture runTexture;

    sf::Texture attackTexture;
    sf::Texture defendTexture;

    sf::Texture jump_startTexture;
    sf::Texture jumpTexture;
    sf::Texture jump_endTexture;

    sf::Texture healTexture;

    std::unique_ptr<sf::Sprite> sprite;
    sf::Vector2f pos;
    sf::Vector2f velocity;

    bool isOnGround = true;
    bool isAttack = false;
    bool isDefend = false;
    bool isJump = false;
    bool isHealing = false;

    float speed = 200.f;       
    float maxSpeed = 550.f;    
    float gravity = 50.f;
    float Groundlevel = 740.f;

    int m_frameNow{ 0 };
    float m_frameCount{ 0.0f };
    float m_framePlus{ 0.2f };

    std::vector<sf::CircleShape> HealSphere;
    int HealsCount = 2;
    std::vector<sf::RectangleShape> HealBar;
    int health = 5;
    static constexpr int MAX_HEALTH = 5;

    sf::CircleShape attackHitbox;
    float attackHitboxRadius = 50.f;
    bool canDamageEnemy = false;

    void SetupPlayer()
    {
        if (!idleTexture.loadFromFile("ASSETS/IMAGES/Sprites/IDLE.png"))
        {
            throw std::runtime_error("Failed to load IDLE.png!");
        }
        if (!runTexture.loadFromFile("ASSETS/IMAGES/Sprites/RUN.png"))
        {
            throw std::runtime_error("Failed to load IDLE.png!");
        }
        if (!attackTexture.loadFromFile("ASSETS/IMAGES/Sprites/ATTACK 1.png"))
        {
            throw std::runtime_error("Failed to load ATTACK 1.png!");
        }
        if (!defendTexture.loadFromFile("ASSETS/IMAGES/Sprites/DEFEND.png"))
        {
            throw std::runtime_error("Failed to load DEFEND.png!");
        }
        if (!jump_startTexture.loadFromFile("ASSETS/IMAGES/Sprites/JUMP-START.png"))
        {
            throw std::runtime_error("Failed to load JUMP start.png!");
        }
        if (!jumpTexture.loadFromFile("ASSETS/IMAGES/Sprites/JUMP.png"))
        {
            throw std::runtime_error("Failed to load JUMP.png!");
        }
        if (!jump_endTexture.loadFromFile("ASSETS/IMAGES/Sprites/JUMP-FALL.png"))
        {
            throw std::runtime_error("Failed to load JUMP end.png!");
        }
        if (!healTexture.loadFromFile("ASSETS/IMAGES/Sprites/HEALING.png"))
        {
            throw std::runtime_error("Failed to load HEal end.png!");
        }

        idleAnim.texture = &idleTexture;
        idleAnim.frameCount = 10;
        idleAnim.frameWidth = 96;
        idleAnim.frameHeight = 96;

        runAnim.texture = &runTexture;
        runAnim.frameCount = 16; 
        runAnim.frameWidth = 96;
        runAnim.frameHeight = 96;

        attackAnim.texture = &attackTexture;
        attackAnim.frameCount = 7;   
        attackAnim.frameWidth = 96;
        attackAnim.frameHeight = 96;

        defendAnim.texture = &defendTexture;
        defendAnim.frameCount = 5;
        defendAnim.frameWidth = 96;
        defendAnim.frameHeight = 96;

        jump_startAnim.texture = &jump_startTexture;
        jump_startAnim.frameCount = 3;
        jump_startAnim.frameWidth = 96;
        jump_startAnim.frameHeight = 96;

        jumpAnim.texture = &jumpTexture;
        jumpAnim.frameCount = 3;
        jumpAnim.frameWidth = 96;
        jumpAnim.frameHeight = 96;

        jump_endAnim.texture = &jump_endTexture;
        jump_endAnim.frameCount = 3;
        jump_endAnim.frameWidth = 96;
        jump_endAnim.frameHeight = 96;
        
        healAnim.texture = &healTexture;
        healAnim.frameCount = 15;
        healAnim.frameWidth = 96;
        healAnim.frameHeight = 96;

        attackHitbox.setRadius(attackHitboxRadius);
        attackHitbox.setOrigin(sf::Vector2f(attackHitboxRadius, attackHitboxRadius));
        attackHitbox.setFillColor(sf::Color(255, 0, 0, 80));

        sprite = std::make_unique<sf::Sprite>(idleTexture);
        sprite->setScale(sf::Vector2f(1.8f, 1.8f));

        pos = { 90.f, 750.f };
        sprite->setPosition(pos);
        sprite->setTextureRect(sf::IntRect{ {0, 0}, {96, 96} });
        sprite->setOrigin(sf::Vector2f(48.f, 48.f)); 
    }

    enum class PlayerState
    {
        Idle,
        Running,
        Jumping,
        Attack,
        Defend,
        Jump_start,
        Jump,
        Jump_end,
        Healing
    };

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
    Animation jump_startAnim;
    Animation jumpAnim;
    Animation jump_endAnim;
    Animation healAnim;
    Animation* currentAnim;
    bool facingRight = true;

    player()
    {
        pos = { 90.f, 750.f };
        velocity = { 0.f, 0.f };
    }

    PlayerState state = PlayerState::Idle;

    void Jump();
    void moveLeft();
    void moveRight();
    void Attack();
    void Defend();
    void Heal();
    void Health();

    void AnimatePlayer();
    void UpdateAnimationTexture();

    void HealCall();
    void TakeDamage(int amount);
    void Update(float dt); 
};


