#include "Enemy1.h"
#include <iostream>

Enemy1::Enemy1()
{
    pos = { 1200.f, 750.f };
    velocity = { 0.f, 0.f };

    // Zero-initialize ALL animation structures (this sets everything to 0/nullptr)
    idleAnim = {};
    runAnim = {};
    attackAnim = {};
    defendAnim = {};

    // Explicitly set texture pointers to nullptr for clarity
    idleAnim.texture = nullptr;
    runAnim.texture = nullptr;
    attackAnim.texture = nullptr;
    defendAnim.texture = nullptr;

    currentAnim = nullptr;

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

    currentAnim = &idleAnim;
    sprite->setTexture(*idleAnim.texture, false);
    sprite->setTextureRect({ {0, 0}, {96, 64} });

    UpdateHealthBar();
}


void Enemy1::SetupEnemy1()
{
    // Setup animations with CORRECT frame widths
    idleAnim.texture = &g_enemyTextures.idle;
    idleAnim.frameCount = 5;
    idleAnim.frameWidth = 96;   // Changed from 48
    idleAnim.frameHeight = 64;

    runAnim.texture = &g_enemyTextures.run;
    runAnim.frameCount = 7;     // Changed from 8
    runAnim.frameWidth = 96;    // Changed from 84
    runAnim.frameHeight = 64;

    attackAnim.texture = &g_enemyTextures.attack;
    attackAnim.frameCount = 5;  // Changed from 6
    attackAnim.frameWidth = 96; // Changed from 80
    attackAnim.frameHeight = 64;

    defendAnim.texture = &g_enemyTextures.defend;
    defendAnim.frameCount = 6;
    defendAnim.frameWidth = 96; // Keep at 96
    defendAnim.frameHeight = 64;

    // Create sprite with correct initial frame
    sprite = std::make_unique<sf::Sprite>(*idleAnim.texture);
    sprite->setScale(sf::Vector2f(2.2f, 2.2f));
    sprite->setPosition(pos);
    sprite->setTextureRect(sf::IntRect({ 0, 0 }, { 96, 64 }));
    sprite->setOrigin(sf::Vector2f(48.f,40.0f)); // Bottom-center of 48x64 frame

    currentAnim = &idleAnim;

    // --- Detection Circles (for debug/visual aid) ---
    detectionRadius.setRadius(detectionRange);
    detectionRadius.setOrigin(sf::Vector2f(detectionRange, detectionRange));
    detectionRadius.setFillColor(sf::Color(255, 255, 0, 40));  // translucent yellow

    attackRadius.setRadius(attackRange);
    attackRadius.setOrigin(sf::Vector2f(attackRange, attackRange));
    attackRadius.setFillColor(sf::Color(255, 0, 0, 60));  // translucent red

    attackHitbox.setRadius(attackHitboxRadius);
    attackHitbox.setOrigin(sf::Vector2f(attackHitboxRadius, attackHitboxRadius));  // Add sf::Vector2f
    attackHitbox.setFillColor(sf::Color(255, 0, 0, 80));

    UpdateHealthBar();
}

void Enemy1::Update(float dt, sf::Vector2f playerPos)
{
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
    // Only allow damage during attack animation frames 2-4 if haven't dealt damage yet
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
    // Enhanced safety checks
    if (!currentAnim) {
        std::cerr << "AnimateEnemy: currentAnim is null!" << std::endl;
        return;
    }

    if (!currentAnim->texture) {
        std::cerr << "AnimateEnemy: texture is null!" << std::endl;
        return;
    }

    // Additional check: verify sprite exists
    if (!sprite) {
        std::cerr << "AnimateEnemy: sprite is null!" << std::endl;
        return;
    }

    m_frameCount += m_framePlus * dt * 60;
    if (m_frameCount >= 1.0f)
    {
        m_frameNow = (m_frameNow + 1) % currentAnim->frameCount;
        m_frameCount = 0.0f;
    }

    if (!currentAnim || !currentAnim->texture)
        return;

    if (currentAnim->frameWidth <= 0 || currentAnim->frameHeight <= 0)
        return;

    // Get frame rect - ADD CLAMPING
    int xPos = m_frameNow * currentAnim->frameWidth;
    int textureWidth = currentAnim->texture->getSize().x;

    // Clamp to prevent reading outside texture
    if (xPos + currentAnim->frameWidth > textureWidth)
        xPos = textureWidth - currentAnim->frameWidth;

    sf::IntRect frameRect(
        sf::Vector2i(xPos, 0),
        sf::Vector2i(currentAnim->frameWidth, currentAnim->frameHeight)
    );
    sprite->setTextureRect(frameRect);

    // Origin at BOTTOM-CENTER (feet position)
    sprite->setOrigin(sf::Vector2f(currentAnim->frameWidth / 2.f, 40.f));

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
        hasDealtDamage = false;  // Reset damage flag
    }

    state = newState;
    Animation* newAnim = nullptr;
    switch (state)
    {
    case EnemyState::Idle:      newAnim = &idleAnim;     break;
    case EnemyState::Running:   newAnim = &runAnim;      break;
    case EnemyState::Attacking: newAnim = &attackAnim;   break;
    case EnemyState::Defending: newAnim = &defendAnim;   break;
    }
    if (currentAnim != newAnim && newAnim != nullptr)
    {
        currentAnim = newAnim;
        sprite->setTexture(*currentAnim->texture, true);
        m_frameNow = 0;
        m_frameCount = 0.f;

        sf::IntRect firstFrame(
            sf::Vector2i(0, 0),
            sf::Vector2i(currentAnim->frameWidth, currentAnim->frameHeight)
        );
        sprite->setTextureRect(firstFrame);
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
    // --- COOLDOWN (stay in idle dont chase) ---
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
        bar.setSize({ 15.f, 15.f });  // Smaller than players
        bar.setFillColor(sf::Color::Red);
        bar.setPosition({ pos.x - 15.f + (i * 18.f), pos.y - 100.f });// Position above the enemy sprite
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