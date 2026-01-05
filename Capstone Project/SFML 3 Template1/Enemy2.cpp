#include "Enemy2.h"
#include <iostream>

Enemy2::Enemy2()
    : pos{ 1200.f, 750.f }
    , velocity{ 0.f, 0.f }
    , health(MAX_HEALTH)
    , state(EnemyState::Idle)
    , facingRight(false)
    , m_frameCount(0.f)
    , m_frameNow(0)
    , m_framePlus(0.05f)
    , speed(80.f)              // Slower than melee (was 150)
    , detectionRange(600.f)     // Longer detection
    , attackRange(400.f)        // Ranged attack
    , preferredDistance(350.f)  // Keeps distance
    , attackCooldown(0.f)
    , attackCooldownTime(2.5f)  // Slower attack rate
    , canDamagePlayer(false)
    , hasDealtDamage(false)
    , attackHitboxRadius(50.f)
    , currentAnim(nullptr)
{
    std::cout << "Enemy2 (Archer) Constructor - pos: " << pos.x << ", " << pos.y << std::endl;
}

void Enemy2::Reset()
{
    health = MAX_HEALTH;
    state = EnemyState::Idle;
    attackCooldown = 0.f;
    hasDealtDamage = false;
    canDamagePlayer = false;
    velocity = { 0.f, 0.f };
    m_frameNow = 0;
    m_frameCount = 0.f;

    // Only update animations if textures have been loaded
    if (idleTexture.getSize().x > 0 && sprite) {
        sprite->setTexture(idleTexture, false);
        sprite->setTextureRect(sf::IntRect(
            sf::Vector2i(0, 0),
            sf::Vector2i(84, 80)  // Correct idle frame size
        ));
        currentAnim = &idleAnim;
    }

    UpdateHealthBar();
}

void Enemy2::SetupEnemy2()
{
    // Load Archer textures
    if (!idleTexture.loadFromFile("ASSETS/IMAGES/Samurai_Archer/IDLE.png"))
        throw std::runtime_error("Failed to load Samurai_Archer/IDLE.png!");
    if (!runTexture.loadFromFile("ASSETS/IMAGES/Samurai_Archer/RUN.png"))
        throw std::runtime_error("Failed to load Samurai_Archer/RUN.png!");
    if (!attackTexture.loadFromFile("ASSETS/IMAGES/Samurai_Archer/ATTACK.png"))
        throw std::runtime_error("Failed to load Samurai_Archer/ATTACK.png!");
    if (!defendTexture.loadFromFile("ASSETS/IMAGES/Samurai_Archer/BLOCK.png"))
        throw std::runtime_error("Failed to load Samurai_Archer/BLOCK.png!");

    std::cout << "Archer IDLE texture: " << idleTexture.getSize().x << " x " << idleTexture.getSize().y << std::endl;
    std::cout << "Archer RUN texture: " << runTexture.getSize().x << " x " << runTexture.getSize().y << std::endl;
    std::cout << "Archer ATTACK texture: " << attackTexture.getSize().x << " x " << attackTexture.getSize().y << std::endl;
    std::cout << "Archer BLOCK texture: " << defendTexture.getSize().x << " x " << defendTexture.getSize().y << std::endl;

    // IDLE: 1344x80 = 14 frames of 96x80
    idleAnim.texture = &idleTexture;
    idleAnim.frameCount = 14;
    idleAnim.frameWidth = 96;
    idleAnim.frameHeight = 80;

    // RUN: 768x80 = 8 frames of 96x80
    runAnim.texture = &runTexture;
    runAnim.frameCount = 8;
    runAnim.frameWidth = 96;
    runAnim.frameHeight = 80;

    // ATTACK: 1056x80 = 11 frames of 96x80
    attackAnim.texture = &attackTexture;
    attackAnim.frameCount = 11;
    attackAnim.frameWidth = 96;
    attackAnim.frameHeight = 80;

    // BLOCK: 576x80 = 6 frames of 96x80
    defendAnim.texture = &defendTexture;
    defendAnim.frameCount = 6;
    defendAnim.frameWidth = 96;
    defendAnim.frameHeight = 80;

    // Create sprite (use idle frame size 84x80)
    sprite = std::make_unique<sf::Sprite>(idleTexture);
    sprite->setScale(sf::Vector2f(2.2f, 2.2f));
    sprite->setPosition(pos);
    sprite->setTextureRect(sf::IntRect({ 0, 0 }, { 96, 80 }));
    sprite->setOrigin(sf::Vector2f(48.f, 60.0f)); // bottom-center of 96x80

    currentAnim = &idleAnim;

    // Detection/Attack circles
    detectionRadius.setRadius(detectionRange);
    detectionRadius.setOrigin(sf::Vector2f(detectionRange, detectionRange));
    detectionRadius.setFillColor(sf::Color(255, 200, 0, 40));  // Orange for archer

    attackRadius.setRadius(attackRange);
    attackRadius.setOrigin(sf::Vector2f(attackRange, attackRange));
    attackRadius.setFillColor(sf::Color(150, 0, 255, 60));  // Purple for archer

    attackHitbox.setRadius(attackHitboxRadius);
    attackHitbox.setOrigin(sf::Vector2f(attackHitboxRadius, attackHitboxRadius));
    attackHitbox.setFillColor(sf::Color(150, 0, 255, 80));

    UpdateHealthBar();
}

void Enemy2::Update(float dt, sf::Vector2f playerPos)
{
    AIBehavior(playerPos, dt);

    pos += velocity * dt;
    sprite->setPosition(pos);

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

    // Archer shoots arrow - hitbox is in front of them
    float hitboxOffsetX = facingRight ? 80.f : -80.f;  // Further out for archer
    attackHitbox.setPosition(sf::Vector2f(pos.x + hitboxOffsetX, pos.y));

    // Reset damage when not attacking
    if (state != EnemyState::Attacking)
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
    if (!currentAnim || !currentAnim->texture)
        return;

    m_frameCount += m_framePlus * dt * 60;
    if (m_frameCount >= 1.0f)
    {
        m_frameNow = (m_frameNow + 1) % currentAnim->frameCount;
        m_frameCount = 0.0f;
    }

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

    // Origin at bottom-center (feet position)
    sprite->setOrigin(sf::Vector2f(42.f, 60.f));

    // Update scale and facing
    float scaleX = facingRight ? std::abs(sprite->getScale().x) : -std::abs(sprite->getScale().x);
    sprite->setScale(sf::Vector2f(scaleX, sprite->getScale().y));
}

void Enemy2::SetState(EnemyState newState)
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

void Enemy2::AIBehavior(sf::Vector2f playerPos, float dt)
{
    float distance = DistanceToPlayer(playerPos);

    // Determine direction
    facingRight = (playerPos.x > pos.x);

    // Decrease cooldown timer
    if (attackCooldown > 0.f)
        attackCooldown -= dt;

    // ARCHER AI: Maintains distance and shoots

    // --- TOO CLOSE - BACK AWAY ---
    if (distance < preferredDistance - 50.f)
    {
        SetState(EnemyState::Running);
        float dir = facingRight ? -1.f : 1.f;  // Move away from player
        velocity.x = dir * speed;
    }
    // --- GOOD RANGE - ATTACK ---
    else if (distance <= attackRange && attackCooldown <= 0.f)
    {
        SetState(EnemyState::Attacking);
        velocity.x = 0.f;  // Stand still to shoot
    }
    // --- COOLDOWN - MAINTAIN DISTANCE ---
    else if (distance <= attackRange && attackCooldown > 0.f)
    {
        SetState(EnemyState::Idle);
        velocity.x = 0.f;
    }
    // --- TOO FAR - MOVE CLOSER ---
    else if (distance > attackRange && distance <= detectionRange)
    {
        SetState(EnemyState::Running);
        float dir = facingRight ? 1.f : -1.f;  // Move toward player
        velocity.x = dir * speed;
    }
    // --- OUT OF RANGE - IDLE ---
    else
    {
        SetState(EnemyState::Idle);
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