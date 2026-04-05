#include "Enemy3.h"
#include <iostream>

Enemy3::Enemy3()
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
    , m_lastAttackUsed(1)
{
    idleAnim = {};
    walkAnim = {};
    attack1Anim = {};
    attack2Anim = {};

    idleAnim.texture = nullptr;
    walkAnim.texture = nullptr;
    attack1Anim.texture = nullptr;
    attack2Anim.texture = nullptr;

    std::cout << "Constructor - Executioner pos: " << pos.x << ", " << pos.y << std::endl;
}

void Enemy3::Reset()
{
    health = MAX_HEALTH;
    state = EnemyState::Idle;
    attackCooldown = 0.f;
    hasDealtDamage = false;
    canDamagePlayer = false;
    velocity = { 0.f, 0.f };

    m_frameNow = 0;
    m_frameCount = 0.f;
    m_lastAttackUsed = 1;

    currentAnimType = AnimationType::Idle;
    sprite->setTexture(*idleAnim.texture, false);
    sprite->setTextureRect({ {0, 0}, {130, 92} });

    UpdateHealthBar();
}

void Enemy3::SetupEnemy3()
{
    // Setup Executioner animations
    // IDLE: 1560 x 92 pixels -> 1560/130 = 12 frames
    idleAnim.texture = &g_executionerTextures.idle;
    idleAnim.frameCount = 12;
    idleAnim.frameWidth = 130;
    idleAnim.frameHeight = 92;

    // WALK: 1560 x 92 pixels -> 1560/130 = 12 frames
    walkAnim.texture = &g_executionerTextures.walk;
    walkAnim.frameCount = 12;
    walkAnim.frameWidth = 130;
    walkAnim.frameHeight = 92;

    // ATTACK 1: 1430 x 92 pixels -> 1430/130 = 11 frames
    attack1Anim.texture = &g_executionerTextures.attack1;
    attack1Anim.frameCount = 11;
    attack1Anim.frameWidth = 130;
    attack1Anim.frameHeight = 92;

    // ATTACK 2: 1170 x 92 pixels -> 1170/130 = 9 frames
    attack2Anim.texture = &g_executionerTextures.attack2;
    attack2Anim.frameCount = 9;
    attack2Anim.frameWidth = 130;
    attack2Anim.frameHeight = 92;

    sprite = std::make_unique<sf::Sprite>(*idleAnim.texture);
    sprite->setScale(sf::Vector2f(2.5f, 2.5f));  // Executioner might be larger
    sprite->setPosition(pos);
    sprite->setTextureRect(sf::IntRect({ 0, 0 }, { 130, 92 }));
    sprite->setOrigin(sf::Vector2f(65.f, 62.f)); 

    currentAnimType = AnimationType::Idle;

    detectionRadius.setRadius(detectionRange);
    detectionRadius.setOrigin(sf::Vector2f(detectionRange, detectionRange));
    detectionRadius.setFillColor(sf::Color(255, 255, 0, 40));

    attackRadius.setRadius(attackRange);
    attackRadius.setOrigin(sf::Vector2f(attackRange, attackRange));
    attackRadius.setFillColor(sf::Color(255, 0, 0, 60));

    attackHitbox.setRadius(attackHitboxRadius);
    attackHitbox.setOrigin(sf::Vector2f(attackHitboxRadius, attackHitboxRadius));
    attackHitbox.setFillColor(sf::Color(255, 0, 0, 80));

    if (!m_stunStarsInitialized)
    {
        m_stunStars.clear();
        for (int i = 0; i < 3; ++i)
        {
            sf::CircleShape star;
            star.setRadius(5.f);
            star.setFillColor(sf::Color::Yellow);
            star.setOutlineColor(sf::Color::Black);
            star.setOutlineThickness(1.f);
            star.setOrigin({ 5.f, 5.f });
            m_stunStars.push_back(star);
        }
        m_stunStarsInitialized = true;
    }

    UpdateHealthBar();
}

Enemy3::Animation* Enemy3::GetCurrentAnimation()
{
    switch (currentAnimType)
    {
    case AnimationType::Idle:       return &idleAnim;
    case AnimationType::Walking:    return &walkAnim;
    case AnimationType::Attack1:    return &attack1Anim;
    case AnimationType::Attack2:    return &attack2Anim;
    default:                         return &idleAnim;
    }
}

void Enemy3::Update(float dt, sf::Vector2f playerPos)
{
    Enemy3::Animation* anim = GetCurrentAnimation();
    assert(sprite && anim && anim->texture);

    // Flash effect when damaged
    if (m_flashTimer > 0.f)
    {
        m_flashTimer -= dt;
        sprite->setColor(sf::Color(255, 0, 0, 255));
    }
    else
    {
        sprite->setColor(sf::Color(255, 255, 255, 255));
    }

    // STUN CHECK
    if (m_isStunned)
    {
        m_stunTimer -= dt;
        if (m_stunTimer <= 0.f)
        {
            m_isStunned = false;
        }

        pos.x += m_knockbackVelocity * dt;

        if (m_knockbackVelocity > 0.f)
        {
            m_knockbackVelocity -= KNOCKBACK_FRICTION * dt;
            if (m_knockbackVelocity < 0.f) m_knockbackVelocity = 0.f;
        }
        else if (m_knockbackVelocity < 0.f)
        {
            m_knockbackVelocity += KNOCKBACK_FRICTION * dt;
            if (m_knockbackVelocity > 0.f) m_knockbackVelocity = 0.f;
        }

        // Update stun stars
        float bounceOffset = std::sin(m_stunTimer * 10.f) * 5.f;
        for (int i = 0; i < m_stunStars.size(); ++i)
        {
            float xOffset = (i - 1) * 20.f;
            m_stunStars[i].setPosition({ pos.x + xOffset, pos.y - 110.f + bounceOffset });
        }

        sprite->setPosition(pos);
        AnimateEnemy(dt);
        UpdateHealthBar();
        return;
    }

    // NORMAL UPDATE
    AIBehavior(playerPos, dt);
    pos += velocity * dt;
    sprite->setPosition(pos);
    AnimateEnemy(dt);
    UpdateHealthBar();

    // Check if attack animation finished
    if ((state == EnemyState::Attack1 || state == EnemyState::Attack2) &&
        m_frameNow >= GetCurrentAnimation()->frameCount - 1)
    {
        float distance = DistanceToPlayer(playerPos);
        if (distance <= detectionRange)
            SetState(EnemyState::Walking);
        else
            SetState(EnemyState::Idle);
    }

    float hitboxOffsetX = facingRight ? 70.f : -70.f;  // Executioner has longer reach
    attackHitbox.setPosition(sf::Vector2f(pos.x + hitboxOffsetX, pos.y));

    // Reset damage when not attacking
    if (state != EnemyState::Attack1 && state != EnemyState::Attack2)
    {
        hasDealtDamage = false;
        canDamagePlayer = false;
    }
    // Allow damage during attack animation frames 3-5 (adjust based on your animation)
    else if (m_frameNow >= 3 && m_frameNow <= 5 && !hasDealtDamage)
    {
        canDamagePlayer = true;
    }
    else
    {
        canDamagePlayer = false;
    }
}

void Enemy3::AnimateEnemy(float dt)
{
    Enemy3::Animation* anim = GetCurrentAnimation();

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

    int xPos = m_frameNow * anim->frameWidth;
    int textureWidth = anim->texture->getSize().x;

    if (xPos + anim->frameWidth > textureWidth)
        xPos = textureWidth - anim->frameWidth;

    sf::Vector2i size(anim->frameWidth, anim->frameHeight);
    sf::Vector2i posRect(xPos, 0);

    // Flip depending on direction
    if (facingRight)
    {
        posRect.x += anim->frameWidth;
        size.x = -anim->frameWidth;
    }

    sf::IntRect frameRect(posRect, size);
    sprite->setTextureRect(frameRect);

    // Keep your origin (this is fine)
    sprite->setOrigin(sf::Vector2f(anim->frameWidth / 2.f, anim->frameHeight - 30.f));;
}

void Enemy3::SetState(EnemyState newState)
{
    if (state == newState)
        return;

    // If leaving attack state, start cooldown
    if ((state == EnemyState::Attack1 || state == EnemyState::Attack2) &&
        (newState != EnemyState::Attack1 && newState != EnemyState::Attack2))
    {
        attackCooldown = attackCooldownTime;
        hasDealtDamage = false;
    }

    state = newState;

    // Update animation TYPE
    AnimationType newAnimType = AnimationType::Idle;
    switch (state)
    {
    case EnemyState::Idle:      newAnimType = AnimationType::Idle;    break;
    case EnemyState::Walking:   newAnimType = AnimationType::Walking; break;
    case EnemyState::Attack1:   newAnimType = AnimationType::Attack1; break;
    case EnemyState::Attack2:   newAnimType = AnimationType::Attack2; break;
    }

    if (currentAnimType != newAnimType)
    {
        currentAnimType = newAnimType;
        m_frameNow = 0;
        m_frameCount = 0.f;

        // Only set texture once
        Enemy3::Animation* newAnim = GetCurrentAnimation();
        if (newAnim && newAnim->texture)
        {
            sprite->setTexture(*newAnim->texture, false); // false = don’t reset rect
            sprite->setTextureRect({ {0, 0}, {newAnim->frameWidth, newAnim->frameHeight} });
        }
    }
}

void Enemy3::AIBehavior(sf::Vector2f playerPos, float dt)
{
    if (m_isStunned) return;

    float distance = DistanceToPlayer(playerPos);

    // Determine facing direction
    facingRight = (playerPos.x > pos.x);

    // Decrease cooldown timer
    if (attackCooldown > 0.f)
        attackCooldown -= dt;

    // --- ATTACK ---
    if ((state != EnemyState::Attack1 && state != EnemyState::Attack2) &&
        distance <= attackRange && attackCooldown <= 0.f)
    {
        // Start attack
        if (m_lastAttackUsed == 1)
        {
            SetState(EnemyState::Attack2);
            m_lastAttackUsed = 2;
        }
        else
        {
            SetState(EnemyState::Attack1);
            m_lastAttackUsed = 1;
        }

        velocity.x = 0.f; // stop moving while attacking
    }
    // --- CHASE ---
    else if (distance > attackRange && distance <= detectionRange)
    {
        SetState(EnemyState::Walking);
        velocity.x = (facingRight ? 1.f : -1.f) * speed;
    }
    // --- IDLE ---
    else if (distance > detectionRange)
    {
        SetState(EnemyState::Idle);
        velocity.x = 0.f;
    }

}
void Enemy3::UpdateHealthBar()
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

void Enemy3::TakeDamage(int amount)
{
    health -= amount;
    if (health < 0) health = 0;

    m_isStunned = true;
    m_stunTimer = STUN_DURATION;
    m_knockbackVelocity = facingRight ? -500.f : 500.f;

    if (state == EnemyState::Attack1 || state == EnemyState::Attack2 || state == EnemyState::Walking)
    {
        SetState(EnemyState::Idle);
    }

    canDamagePlayer = false;
    hasDealtDamage = true;

    m_flashTimer = FLASH_DURATION;

    UpdateHealthBar();
}

float Enemy3::DistanceToPlayer(sf::Vector2f playerPos)
{
    float dx = playerPos.x - pos.x;
    float dy = playerPos.y - pos.y;
    return std::sqrt(dx * dx + dy * dy);
}