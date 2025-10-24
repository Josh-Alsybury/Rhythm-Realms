#include "Player.h"
#include <cmath> 


void player::Jump()
{
    if (isOnGround)
        velocity.y -= 90;
        isJump = true;
        isOnGround = false;
        state = PlayerState::Jump_start;
}

void player::moveLeft()
{
    velocity.x -= speed;
    if (facingRight) {
        facingRight = false;
        sprite->setScale(sf::Vector2f( - std::abs(sprite->getScale().x), sprite->getScale().y));
    }
}

void player::moveRight()
{
    velocity.x += speed;
    if (!facingRight) {
        facingRight = true;
        sprite->setScale(sf::Vector2f(std::abs(sprite->getScale().x), sprite->getScale().y));
    }
}

void player::Attack()
{
    if(velocity.x == 0.f)
        if (isOnGround)
            isAttack = true;
}

void player::Defend()
{
    if (velocity.x == 0.f)
        if (isOnGround)
            isDefend = true;
}

void player::Heal()
{
    if (isOnGround && !isHealing && HealsCount > 0)
    {
        isHealing = true;
        state = PlayerState::Healing;
    }
}


void player::AnimatePlayer()
{
    UpdateAnimationTexture();

    m_frameCount += m_framePlus;
    int frame = static_cast<int>(m_frameCount) % currentAnim->frameCount;

    if (frame != m_frameNow)
    {
        m_frameNow = frame;
        sf::IntRect rect(
            sf::Vector2i(frame * currentAnim->frameWidth, 0),             
            sf::Vector2i(currentAnim->frameWidth, currentAnim->frameHeight) 
        );
        sprite->setTextureRect(rect);
        sprite->setOrigin(sf::Vector2f(currentAnim->frameWidth / 2.f, currentAnim->frameHeight / 2.f));
    }

    if ((state == PlayerState::Attack && m_frameNow == attackAnim.frameCount - 1) ||
        (state == PlayerState::Defend && m_frameNow == defendAnim.frameCount - 1) ||
        (state == PlayerState::Healing && m_frameNow == healAnim.frameCount - 1))
    {
        if (state == PlayerState::Healing)
        {
            if (HealsCount > 0 || health != MAX_HEALTH)
            {
                HealsCount -= 1;
                health = MAX_HEALTH;
                HealCall();
                Health();
            }
        }

        isAttack = false;
        isDefend = false;
        isHealing = false;
    }
}

void player::UpdateAnimationTexture()
{
    Animation* newAnim = nullptr;

    switch (state)
    {
    case PlayerState::Idle:  newAnim = &idleAnim; break;
    case PlayerState::Running: newAnim = &runAnim; break;
    case PlayerState::Attack: newAnim = &attackAnim; break;
    case PlayerState::Defend: newAnim = &defendAnim; break;
    case PlayerState::Jump_start: newAnim = &jump_startAnim; break;
    case PlayerState::Jump: newAnim = &jumpAnim; break;
    case PlayerState::Jump_end: newAnim = &jump_endAnim; break;
    case PlayerState::Healing: newAnim = &healAnim; break;
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
        sprite->setOrigin(sf::Vector2f(currentAnim->frameWidth / 2.f, currentAnim->frameHeight / 2.f));
    }
}

void player::Health()
{
    HealBar.clear();

    for (int i = 0; i < health; ++i)
    {
        sf::RectangleShape bar;
        bar.setSize({ 25.f, 25.f });
        bar.setFillColor(sf::Color::Red);
        bar.setPosition({ 10.f + (i * 32.f), 15.f });
        HealBar.push_back(bar);
    }
}

void player::HealCall()
{
    if (HealSphere.size() != HealsCount)
    {
        HealSphere.clear();
        HealSphere.resize(HealsCount);

        for (int i = 0; i < HealsCount; i++)
        {
            HealSphere[i].setRadius(10);
            HealSphere[i].setFillColor(sf::Color::White);
            HealSphere[i].setPosition({ 10 + (i * 32.f),  50.f });
        }
    }
}

void player::TakeDamage(int amount)
{
    health -= 1;
    if (health < 0) health = 0;
    Health();
}

void player::Update(float dt)
{
    if (!isOnGround)
    {
        if (state == PlayerState::Jump_start)
        {
            if (m_frameNow >= jump_startAnim.frameCount - 1 && velocity.y >= 0)
            {
                state = PlayerState::Jump_end;
            }
        }
        else if (velocity.y > 0)
        {
            state = PlayerState::Jump_end;
        }
    }
    else
    {
        if (isJump)
        {
            isJump = false;
        }
        if (isAttack)
            state = PlayerState::Attack;
        else if (isDefend)
            state = PlayerState::Defend;
        else if (isHealing)
            state = PlayerState::Healing;
        else if (std::abs(velocity.x) > 0)
            state = PlayerState::Running;
        else
            state = PlayerState::Idle;
    }
    AnimatePlayer();

 
    if (velocity.x > maxSpeed) velocity.x = maxSpeed;
    if (velocity.x < -maxSpeed) velocity.x = -maxSpeed;

    velocity.y += gravity * dt;

    if (isOnGround)
    {
        if (velocity.x > 0)
            velocity.x -= friction * dt;
        else if (velocity.x < 0)
            velocity.x += friction * dt;

        if (std::abs(velocity.x) < 1.f)
            velocity.x = 0.f;
    }

    pos += velocity * dt;
    sprite->setPosition(pos);

    if (pos.y >= Groundlevel)
    {
        pos.y = Groundlevel;
        velocity.y = 0;
        isOnGround = true;
    }
    else
    {
        isOnGround = false;
    }

    sprite->setPosition(pos);
}

