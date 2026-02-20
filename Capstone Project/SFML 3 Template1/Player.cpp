#include "Headers/Player.h"
#include <cmath> 


void player::Jump()
{
    if (isHealing == false)
    {
        if (isOnGround)
            velocity.y -= 120;
        isJump = true;
        isOnGround = false;
        state = PlayerState::Jump_start;
    }
}

void player::moveLeft()
{
    if (isHealing == false)
    {
        velocity.x =  -speed;
        if (facingRight) {
            facingRight = false;
            sprite->setScale(sf::Vector2f(-std::abs(sprite->getScale().x), sprite->getScale().y));
        }
    }
}

void player::moveRight()
{
    if (isHealing == false)
    {
        velocity.x = +speed;
        if (!facingRight) {
            facingRight = true;
            sprite->setScale(sf::Vector2f(std::abs(sprite->getScale().x), sprite->getScale().y));
        }
    }
}

void player::Attack()
{
    if (velocity.x == 0.f)
        if (isOnGround)
        {
            isAttack = true;

            if (m_bpmSystem)
            {
                BPMCombatSystem::HitTiming timing = m_bpmSystem->evaluateHitTiming();
                m_lastAttackTiming = m_bpmSystem->registerHit(timing);

                ShowTimingFeedback(timing);
            }
        }
}

void player::Defend()
{
    if (velocity.x == 0.f)
        if (isOnGround)
        {
            isDefend = true;

            if (m_bpmSystem)
            {
                BPMCombatSystem::HitTiming timing = m_bpmSystem->evaluateHitTiming();
                m_lastBlockTiming = m_bpmSystem->registerBlock(timing);

                if (m_lastBlockTiming == -1.0f)
                {
                    m_isPerfectParry = true;
                }
                ShowTimingFeedback(timing);
            }
        }
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
    // Update knockback timer
    if (isKnockedBack)
    {
        knockbackTimer -= dt;
        if (knockbackTimer <= 0.0f)
        {
            isKnockedBack = false;
        }
    }

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

    if (!isOnGround)
    {
        velocity.y += gravity * dt;
    }

    pos.x += velocity.x * dt;
    if (!isOnGround)
    {
        pos.y += velocity.y * dt;
    }

    sprite->setPosition(pos);


    if (!isKnockedBack)  // Knockback condition
    {
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
        {
            velocity.x = 0.f;  // Instant stop
        }
    }

    float hitboxOffsetX = facingRight ? 60.f : -60.f;
    attackHitbox.setPosition(sf::Vector2f(pos.x + hitboxOffsetX, pos.y));

    // Only allow damage during attack anim frame 3-5
    if (state == PlayerState::Attack && m_frameNow >= 3 && m_frameNow <= 5)
        canDamageEnemy = true;
    else
        canDamageEnemy = false;
    // Only allow block during defend animframe 1-3
    if (state == PlayerState::Defend && m_frameNow >= 1 && m_frameNow <= 5)
        canBlockEnemy = true;
    else
        canBlockEnemy = false;


    sprite->setPosition(pos);
}

void player::InitializeBPMVisuals(const sf::Font& font)
{
    
    m_timingFeedbackText.emplace(font, "", 32);
    m_timingFeedbackText->setFillColor(sf::Color::Yellow);
    m_timingFeedbackText->setOutlineColor(sf::Color::Black);
    m_timingFeedbackText->setOutlineThickness(2.f);

    //  beat indicator 
    m_beatIndicator.setRadius(20.f);
    m_beatIndicator.setFillColor(sf::Color(255, 255, 255, 100));
    m_beatIndicator.setOutlineColor(sf::Color::Cyan);
    m_beatIndicator.setOutlineThickness(3.f);
    m_beatIndicator.setOrigin({ 20.f, 20.f });
}

void player::UpdateBPMVisuals(float dt, float currentBPM)
{
    // Timing feedback timer
    if (m_timingFeedbackTimer > 0.f)
    {
        m_timingFeedbackTimer -= dt;

        // Fade out effect
        if (m_timingFeedbackText.has_value())
        {
            float alpha = std::clamp(m_timingFeedbackTimer / TIMING_FEEDBACK_DURATION, 0.f, 1.f);
            sf::Color color = m_timingFeedbackText->getFillColor();
            color.a = static_cast<std::uint8_t>(255 * alpha);  // SFML 3.0: std::uint8_t
            m_timingFeedbackText->setFillColor(color);
        }
    }

    // Beat pulse animation
    if (currentBPM > 0.f)
    {
        float beatPeriod = 60.f / currentBPM;
        static float beatTimer = 0.f;
        beatTimer += dt;

        if (beatTimer >= beatPeriod)
        {
            beatTimer -= beatPeriod;
            m_beatPulseScale = 1.5f;  // Pulse on beat
        }

        m_beatPulseScale += (1.0f - m_beatPulseScale) * 8.0f * dt;
    }
}

void player::RenderBPMVisualsAtPosition(sf::RenderWindow& window, const sf::Vector2f& screenPos)
{
    // Beat indicator 
    m_beatIndicator.setPosition({ screenPos.x, screenPos.y - 80.f });
    m_beatIndicator.setScale({ m_beatPulseScale, m_beatPulseScale });
    window.draw(m_beatIndicator);

    // Timing feedback text
    if (m_timingFeedbackTimer > 0.f && m_timingFeedbackText.has_value())
    {
        sf::FloatRect textBounds = m_timingFeedbackText->getLocalBounds();
        m_timingFeedbackText->setPosition({
            screenPos.x - textBounds.size.x / 2.f,
            screenPos.y - 120.f
            });
        window.draw(*m_timingFeedbackText);
    }
}

// Keep the old method for backwards compatibility
void player::RenderBPMVisuals(sf::RenderWindow& window)
{
    RenderBPMVisualsAtPosition(window, pos);
}

void player::ShowTimingFeedback(BPMCombatSystem::HitTiming timing)
{
    if (!m_timingFeedbackText.has_value()) return;

    m_timingFeedbackTimer = TIMING_FEEDBACK_DURATION;

    switch (timing)
    {
    case BPMCombatSystem::HitTiming::Perfect:
        m_timingFeedbackText->setString("PERFECT!");
        m_timingFeedbackText->setFillColor(sf::Color(255, 215, 0));  // Gold
        break;

    case BPMCombatSystem::HitTiming::Good:
        m_timingFeedbackText->setString("Good");
        m_timingFeedbackText->setFillColor(sf::Color::Green);
        break;

    case BPMCombatSystem::HitTiming::Miss:
        m_timingFeedbackText->setString("Miss");
        m_timingFeedbackText->setFillColor(sf::Color::Red);
        break;
    }
}

void player::setBPMSystem(BPMCombatSystem* system)
{
    m_bpmSystem = system;
}