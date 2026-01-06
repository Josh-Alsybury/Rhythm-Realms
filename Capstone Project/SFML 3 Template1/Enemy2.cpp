#include "Enemy2.h"
#include <iostream>

Enemy2::Enemy2()
    : pos{ 1200.f, 750.f }
    , velocity{ 0.f, 0.f }
    , health(MAX_HEALTH)
    , state(ArcherState::Idle)
    , facingRight(false)
    , m_frameCount(0.f)
    , m_frameNow(0)
    , m_framePlus(0.05f)
    , speed(80.f)
    , detectionRange(600.f)     
    , attackRange(550.f)         
    , preferredDistance(420.f)
    , attackCooldown(0.f)
    , attackCooldownTime(2.5f)
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

    // Explicitly set texture pointers to nullptr for clarity
    idleAnim.texture = nullptr;
    runAnim.texture = nullptr;
    attackAnim.texture = nullptr;
    defendAnim.texture = nullptr;
}

void Enemy2::Reset()
{
    health = MAX_HEALTH;
    state = ArcherState::Idle;
    attackCooldown = 0.f;
    hasDealtDamage = false;
    canDamagePlayer = false;
    velocity = { 0.f, 0.f };

    m_frameNow = 0;
    m_frameCount = 0.f;

    // Reset to idle animation
    currentAnimType = AnimationType::Idle;

    UpdateHealthBar();
}

void Enemy2::SetupEnemy2()
{
    // Use GLOBAL textures (already loaded in Game constructor)
    idleAnim.texture = &g_archerTextures.idle;
    idleAnim.frameCount = 14;
    idleAnim.frameWidth = 96;
    idleAnim.frameHeight = 80;

    runAnim.texture = &g_archerTextures.run;
    runAnim.frameCount = 8;
    runAnim.frameWidth = 96;
    runAnim.frameHeight = 80;

    attackAnim.texture = &g_archerTextures.attack;
    attackAnim.frameCount = 11;
    attackAnim.frameWidth = 96;
    attackAnim.frameHeight = 80;

    defendAnim.texture = &g_archerTextures.defend;
    defendAnim.frameCount = 6;
    defendAnim.frameWidth = 96;
    defendAnim.frameHeight = 80;

    // Create sprite
    sprite = std::make_unique<sf::Sprite>(*idleAnim.texture);
    sprite->setScale(sf::Vector2f(2.2f, 2.2f));
    sprite->setPosition(pos);
    sprite->setTextureRect(sf::IntRect({ 0, 0 }, { 96, 80 }));
    sprite->setOrigin(sf::Vector2f(48.f, 50.0f));

    currentAnimType = AnimationType::Idle;
    isInitialised = true;

    // Detection/Attack circles
    detectionRadius.setRadius(detectionRange);
    detectionRadius.setOrigin(sf::Vector2f(detectionRange, detectionRange));
    detectionRadius.setFillColor(sf::Color(255, 200, 0, 40));

    attackRadius.setRadius(attackRange);
    attackRadius.setOrigin(sf::Vector2f(attackRange, attackRange));
    attackRadius.setFillColor(sf::Color(150, 0, 255, 60));

    attackHitbox.setRadius(attackHitboxRadius);
    attackHitbox.setOrigin(sf::Vector2f(attackHitboxRadius, attackHitboxRadius));
    attackHitbox.setFillColor(sf::Color(150, 0, 255, 80));

    detectionRadius.setRadius(detectionRange); 
    attackRadius.setRadius(attackRange);

    UpdateHealthBar();
}

// HELPER: Get current animation by type (safe from vector reallocation)
Enemy2::Animation* Enemy2::GetCurrentAnimation()
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

void Enemy2::Update(float dt, sf::Vector2f playerPos)
{
    if (!isInitialised)
        return;

    // Get animation fresh every frame - safe from reallocation
    Enemy2::Animation* anim = GetCurrentAnimation();

    assert(sprite && anim && anim->texture);

    AIBehavior(playerPos, dt);

    pos += velocity * dt;
    sprite->setPosition(pos);

    AnimateEnemy(dt);
    UpdateHealthBar();

    // Check if attack animation finished
    if (state == ArcherState::Attacking && m_frameNow >= attackAnim.frameCount - 1)
    {
        float distance = DistanceToPlayer(playerPos);
        if (distance <= detectionRange)
            SetState(ArcherState::Running);
        else
            SetState(ArcherState::Idle);
    }

    // Archer shoots arrow - hitbox is in front of them
    float hitboxOffsetX = facingRight ? 80.f : -80.f;
    attackHitbox.setPosition(sf::Vector2f(pos.x + hitboxOffsetX, pos.y));

    // Reset damage when not attacking
    if (state != ArcherState::Attacking)
    {
        hasDealtDamage = false;
        canDamagePlayer = false;
    }
    // Archer damages during specific attack frames (when arrow is released)
    else if (m_frameNow >= 7 && m_frameNow <= 9 && !hasDealtDamage)
    {
        canDamagePlayer = true;
    }
    else
    {
        canDamagePlayer = false;
    }
}

void Enemy2::AnimateEnemy(float dt)
{
    if (!isInitialised)
        return;

    // Get animation fresh - safe from reallocation
    Enemy2::Animation* anim = GetCurrentAnimation();

    if (!anim || !anim->texture || !sprite)
        return;

    if (anim->frameCount <= 0)
        return;

    m_frameCount += m_framePlus * dt * 60.f;
    if (m_frameCount >= 1.0f)
    {
        m_frameNow = (m_frameNow + 1) % anim->frameCount;
        m_frameCount = 0.0f;
    }

    if (anim->frameWidth <= 0 || anim->frameHeight <= 0)
        return;

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

    // Origin at bottom-center (feet position)
    sprite->setOrigin(sf::Vector2f(48.f, 50.f));

    // Update scale and facing
    float scaleX = facingRight ? std::abs(sprite->getScale().x) : -std::abs(sprite->getScale().x);
    sprite->setScale(sf::Vector2f(scaleX, sprite->getScale().y));
}

void Enemy2::SetState(ArcherState newState)
{
    if (!isInitialised)
        return;

    if (state == newState)
        return;

    // cooldown handling
    if (state == ArcherState::Attacking && newState != ArcherState::Attacking)
    {
        attackCooldown = attackCooldownTime;
        hasDealtDamage = false;
    }

    state = newState;

    // Update animation TYPE (not pointer!)
    AnimationType newAnimType = AnimationType::Idle;
    switch (state)
    {
    case ArcherState::Idle:      newAnimType = AnimationType::Idle;      break;
    case ArcherState::Running:   newAnimType = AnimationType::Running;   break;
    case ArcherState::Attacking: newAnimType = AnimationType::Attacking; break;
    case ArcherState::Defending: newAnimType = AnimationType::Defending; break;
    }

    if (currentAnimType != newAnimType)
    {
        currentAnimType = newAnimType;
        m_frameNow = 0;
        m_frameCount = 0.f;

        // Get the new animation and update sprite
        Enemy2::Animation* newAnim = GetCurrentAnimation();
        if (newAnim && newAnim->texture && newAnim->frameCount > 0)
        {
            sprite->setTexture(*newAnim->texture, false);
            sprite->setTextureRect({
                {0, 0},
                {newAnim->frameWidth, newAnim->frameHeight}
                });
        }
    }
}

void Enemy2::AIBehavior(sf::Vector2f playerPos, float dt)
{
    float distance = DistanceToPlayer(playerPos);

    // Determine direction
    facingRight = (playerPos.x > pos.x);

    // Decrease cooldown timer
    if (attackCooldown > 0.f)
        attackCooldown -= dt;

    // --- TOO CLOSE - BACK AWAY ---
    if (distance < preferredDistance - 80.f)
    {
        SetState(ArcherState::Running);
        float dir = facingRight ? -1.f : 1.f;  // Move away from player
        velocity.x = dir * speed;
    }
    // --- GOOD RANGE - ATTACK ---
    else if (distance <= attackRange && distance >= preferredDistance - 20.f && attackCooldown <= 0.f)
    {
        SetState(ArcherState::Attacking);
        velocity.x = 0.f;  // Stand still to shoot
    }
    // --- COOLDOWN - MAINTAIN DISTANCE ---
    else if (distance <= attackRange && attackCooldown > 0.f)
    {
        SetState(ArcherState::Idle);
        velocity.x = 0.f;
    }
    // --- TOO FAR - MOVE CLOSER ---
    else if (distance > attackRange && distance <= detectionRange)
    {
        SetState(ArcherState::Running);
        float dir = facingRight ? 1.f : -1.f;  // Move toward player
        velocity.x = dir * speed;
    }
    // --- OUT OF RANGE - IDLE ---
    else
    {
        SetState(ArcherState::Idle);
        velocity.x = 0.f;
    }
}

void Enemy2::UpdateHealthBar()
{
    healthBar.clear();

    for (int i = 0; i < health; ++i)
    {
        sf::RectangleShape bar;
        bar.setSize({ 15.f, 15.f });
        bar.setFillColor(sf::Color(150, 0, 255));  // Purple health for archer
        bar.setPosition({ pos.x - 15.f + (i * 18.f), pos.y - 120.f });  // Higher up for archer
        healthBar.push_back(bar);
    }
}

void Enemy2::TakeDamage(int amount)
{
    health -= amount;
    if (health < 0) health = 0;
    UpdateHealthBar();
}

float Enemy2::DistanceToPlayer(sf::Vector2f playerPos)
{
    float dx = playerPos.x - pos.x;
    float dy = playerPos.y - pos.y;
    return std::sqrt(dx * dx + dy * dy);
}