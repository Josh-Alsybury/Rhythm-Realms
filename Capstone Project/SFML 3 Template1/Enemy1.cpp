#include "Enemy1.h"
#include <iostream>

Enemy1::Enemy1()
    : pos{ 1200.f, 750.f }
    , velocity{ 0.f, 0.f }
    , health(MAX_HEALTH)
    , state(EnemyState::Idle)
    , facingRight(false)
    , m_frameCount(0.f)
    , m_frameNow(0)
    , m_framePlus(0.2f)
    , speed(70.f)
    , attackCooldown(0.f)
    , attackCooldownTime(1.0f)
    , canDamagePlayer(false)
    , hasDealtDamage(false)
    , attackHitboxRadius(50.f)
    , currentAnimType(AnimationType::Idle)
{
    // Zero-initialize ALL animation structures
    idleAnim = {};
    runAnim = {};
    attackAnim = {};
    defendAnim = {};

    // Explicitly set texture pointers to nullptr
    idleAnim.texture = nullptr;
    runAnim.texture = nullptr;
    attackAnim.texture = nullptr;
    defendAnim.texture = nullptr;

    std::cout << "Constructor - Enemy pos: " << pos.x << ", " << pos.y << std::endl;
}

void Enemy1::Reset()
{
    health = MAX_HEALTH;
    state = EnemyState::Idle;
    attackCooldown = 0.f;
    hasDealtDamage = false;
    canDamagePlayer = false;
    velocity = { 0.f, 0.f };

    m_frameNow = 0;
    m_frameCount = 0.f;

    // Reset to idle animation
    currentAnimType = AnimationType::Idle;
    sprite->setTexture(*idleAnim.texture, false);
    sprite->setTextureRect({ {0, 0}, {96, 64} });

    UpdateHealthBar();
}

void Enemy1::SetupEnemy1()
{
    // Setup animations with CORRECT frame widths
    idleAnim.texture = &g_samuraiTextures.idle;
    idleAnim.frameCount = 5;
    idleAnim.frameWidth = 96;
    idleAnim.frameHeight = 64;

    runAnim.texture = &g_samuraiTextures.run;
    runAnim.frameCount = 7;
    runAnim.frameWidth = 96;
    runAnim.frameHeight = 64;

    attackAnim.texture = &g_samuraiTextures.attack;
    attackAnim.frameCount = 5;
    attackAnim.frameWidth = 96;
    attackAnim.frameHeight = 64;

    defendAnim.texture = &g_samuraiTextures.defend;
    defendAnim.frameCount = 6;
    defendAnim.frameWidth = 96;
    defendAnim.frameHeight = 64;

    // Create sprite with correct initial frame
    sprite = std::make_unique<sf::Sprite>(*idleAnim.texture);
    sprite->setScale(sf::Vector2f(2.2f, 2.2f));
    sprite->setPosition(pos);
    sprite->setTextureRect(sf::IntRect({ 0, 0 }, { 96, 64 }));
    sprite->setOrigin(sf::Vector2f(48.f, 30.f));

    currentAnimType = AnimationType::Idle;

    // Detection/Attack circles
    detectionRadius.setRadius(detectionRange);
    detectionRadius.setOrigin(sf::Vector2f(detectionRange, detectionRange));
    detectionRadius.setFillColor(sf::Color(255, 255, 0, 40));

    attackRadius.setRadius(attackRange);
    attackRadius.setOrigin(sf::Vector2f(attackRange, attackRange));
    attackRadius.setFillColor(sf::Color(255, 0, 0, 60));

    attackHitbox.setRadius(attackHitboxRadius);
    attackHitbox.setOrigin(sf::Vector2f(attackHitboxRadius, attackHitboxRadius));
    attackHitbox.setFillColor(sf::Color(255, 0, 0, 80));

    UpdateHealthBar();
}

// HELPER: Get current animation by type (safe from vector reallocation)
Enemy1::Animation* Enemy1::GetCurrentAnimation()
{
    switch (currentAnimType)
    {
    case AnimationType::Idle:      return &idleAnim;
    case AnimationType::Running:   return &runAnim;
    case AnimationType::Attacking: return &attackAnim;
    case AnimationType::Defending: return &defendAnim;
    default:                        return &idleAnim;
    }
}

void Enemy1::Update(float dt, sf::Vector2f playerPos)
{
    // Get animation fresh every frame - safe from reallocation
    Enemy1::Animation* anim = GetCurrentAnimation();

    assert(sprite && anim && anim->texture);

    AIBehavior(playerPos, dt);

    pos += velocity * dt;

    AnimateEnemy(dt);
    UpdateHealthBar();

    // Check if attack animation finished
    if (state == EnemyState::Attacking && m_frameNow >= attackAnim.frameCount - 1)
    {
        float distance = DistanceToPlayer(playerPos);
        if (distance <= detectionRange)
            SetState(EnemyState::Running);
        else
            SetState(EnemyState::Idle);
    }

    float hitboxOffsetX = facingRight ? 60.f : -60.f;
    attackHitbox.setPosition(sf::Vector2f(pos.x + hitboxOffsetX, pos.y));

    // Reset damage when not attacking
    if (state != EnemyState::Attacking)
    {
        hasDealtDamage = false;
        canDamagePlayer = false;
    }
    // Only allow damage during attack animation frames 2-4
    else if (m_frameNow >= 2 && m_frameNow <= 4 && !hasDealtDamage)
    {
        canDamagePlayer = true;
    }
    else
    {
        canDamagePlayer = false;
    }
}

void Enemy1::AnimateEnemy(float dt)
{
    // Get animation fresh - safe from reallocation
    Enemy1::Animation* anim = GetCurrentAnimation();

    if (!anim || !anim->texture || !sprite)
        return;

    if (anim->frameCount <= 0)
        return;

    m_frameCount += m_framePlus * dt * 60;
    if (m_frameCount >= 1.0f)
    {
        m_frameNow = (m_frameNow + 1) % anim->frameCount;
        m_frameCount = 0.0f;
    }

    if (anim->frameWidth <= 0 || anim->frameHeight <= 0)
        return;

    // Get frame rect with clamping
    int xPos = m_frameNow * anim->frameWidth;
    int textureWidth = anim->texture->getSize().x;

    // Clamp to prevent reading outside texture
    if (xPos + anim->frameWidth > textureWidth)
        xPos = textureWidth - anim->frameWidth;

    sf::IntRect frameRect(
        sf::Vector2i(xPos, 0),
        sf::Vector2i(anim->frameWidth, anim->frameHeight)
    );
    sprite->setTextureRect(frameRect);

    // Origin at BOTTOM-CENTER (feet position)
    sprite->setOrigin(sf::Vector2f(anim->frameWidth / 2.f, 30.f));

    // Update scale and facing
    float scaleX = facingRight ? std::abs(sprite->getScale().x) : -std::abs(sprite->getScale().x);
    sprite->setScale(sf::Vector2f(scaleX, sprite->getScale().y));
}

void Enemy1::SetState(EnemyState newState)
{
    if (state == newState)
        return;

    // If leaving attack state, start cooldown
    if (state == EnemyState::Attacking && newState != EnemyState::Attacking)
    {
        attackCooldown = attackCooldownTime;
        hasDealtDamage = false;
    }

    state = newState;

    // Update animation TYPE (not pointer!)
    AnimationType newAnimType = AnimationType::Idle;
    switch (state)
    {
    case EnemyState::Idle:      newAnimType = AnimationType::Idle;      break;
    case EnemyState::Running:   newAnimType = AnimationType::Running;   break;
    case EnemyState::Attacking: newAnimType = AnimationType::Attacking; break;
    case EnemyState::Defending: newAnimType = AnimationType::Defending; break;
    }

    if (currentAnimType != newAnimType)
    {
        currentAnimType = newAnimType;
        m_frameNow = 0;
        m_frameCount = 0.f;

        // Get the new animation and update sprite
        Enemy1::Animation* newAnim = GetCurrentAnimation();
        if (newAnim && newAnim->texture && newAnim->frameCount > 0)
        {
            sprite->setTexture(*newAnim->texture, true);
            sprite->setTextureRect({
                {0, 0},
                {newAnim->frameWidth, newAnim->frameHeight}
                });
        }
    }
}

void Enemy1::AIBehavior(sf::Vector2f playerPos, float dt)
{
    float distance = DistanceToPlayer(playerPos);

    // Determine direction
    facingRight = (playerPos.x > pos.x);

    // Decrease cooldown timer
    if (attackCooldown > 0.f)
        attackCooldown -= dt;

    // --- ATTACK ---
    if (distance <= attackRange && attackCooldown <= 0.f)
    {
        SetState(EnemyState::Attacking);
        velocity.x = 0.f;
    }
    // --- COOLDOWN (stay in idle, don't chase) ---
    else if (distance <= attackRange && attackCooldown > 0.f)
    {
        SetState(EnemyState::Idle);
        velocity.x = 0.f;
    }
    // --- CHASE ---
    else if (distance <= detectionRange)
    {
        SetState(EnemyState::Running);
        float dir = facingRight ? 1.f : -1.f;
        velocity.x = dir * speed;
    }
    // --- IDLE ---
    else
    {
        SetState(EnemyState::Idle);
        velocity.x = 0.f;
    }

    pos.x += velocity.x * dt;
}

void Enemy1::UpdateHealthBar()
{
    healthBar.clear();

    for (int i = 0; i < health; ++i)
    {
        sf::RectangleShape bar;
        bar.setSize({ 15.f, 15.f });
        bar.setFillColor(sf::Color::Red);
        bar.setPosition({ pos.x - 15.f + (i * 18.f), pos.y - 100.f });
        healthBar.push_back(bar);
    }
}

void Enemy1::TakeDamage(int amount)
{
    health -= amount;
    if (health < 0) health = 0;
    UpdateHealthBar();
}

float Enemy1::DistanceToPlayer(sf::Vector2f playerPos)
{
    float dx = playerPos.x - pos.x;
    float dy = playerPos.y - pos.y;
    return std::sqrt(dx * dx + dy * dy);
}