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
    if (velocity.x == 0.f && isOnGround)
    {
        isAttack = true;

        float lungeSpeed = 150.f;
        m_attackMomentum.x = facingRight ? lungeSpeed : -lungeSpeed;

        if (m_bpmSystem)
        {
            BPMCombatSystem::HitTiming timing = m_bpmSystem->evaluateHitTiming();

            if (timing == BPMCombatSystem::HitTiming::Perfect)
            {
                m_damageMultiplier = 2.0f;
                m_attackMomentum.x *= 1.5f;  
                m_comboCount++;
                m_comboTimer = COMBO_TIMEOUT;
                ShowTimingFeedback(timing);
            }
            else if (timing == BPMCombatSystem::HitTiming::Good)
            {
                m_damageMultiplier = 1.5f;
                m_attackMomentum.x *= 1.2f;  
                m_comboCount++;
                m_comboTimer = COMBO_TIMEOUT;
                ShowTimingFeedback(timing);
            }
            else
            {
                m_damageMultiplier = 1.0f;
            }
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

                    if (m_hasPerfectParry)
                    {
                        int flip = rand() % 2;
                        if (flip == 0)
                        {
                            m_parrySpeedBurst = true;
                            std::cout << "Perfect Parry: SPEED BURST!" << std::endl;
                        }
                        else
                        {
                            m_parryPowerHit = true;
                            std::cout << "Perfect Parry: POWER HIT READY!" << std::endl;
                        }
                    }
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

    if (state == PlayerState::Attack)
        m_framePlus = m_hasAttackSpeed ? 0.35f : 0.2f;
    else
        m_framePlus = 0.2f;

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

    if ((state == PlayerState::Attack && m_frameNow == attackAnim.frameCount - 1)||
        (state == PlayerState::Defend && m_frameNow == defendAnim.frameCount - 1) ||
        (state == PlayerState::Healing && m_frameNow == healAnim.frameCount - 1))
    {
        if (state == PlayerState::Healing)
        {
            if (HealsCount > 0 || health != MAX_HEALTH)
            {
                HealsCount -= 1;
                health = std::min(health + 40, static_cast<int>(MAX_HEALTH * m_overhealCap));
                HealCall();
                Health();
            }
        }
        if (state == PlayerState::Attack)
        {
            m_hasHitThisAttack = false;

        }

        if (state == PlayerState::Defend && m_parrySpeedBurst)
        {
            float dir = facingRight ? 1.f : -1.f;
            velocity.x = dir * 400.f;
            m_parrySpeedBurst = false;
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
    sf::RectangleShape backgroud;
    backgroud.setSize({200.f,20.f});
    backgroud.setFillColor( sf::Color(60, 60, 60) );
    backgroud.setPosition({ 10.f, 15.f });
    HealBar.push_back(backgroud);

    float ratio = static_cast<float>(health) / MAX_HEALTH;
    sf::RectangleShape foreground;
    foreground.setSize({ 200.f * ratio, 20.f });
    foreground.setFillColor(sf::Color(200, 30, 30));
    foreground.setPosition({ 10.f, 15.f });
    HealBar.push_back(foreground);
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
    health -= amount;
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

    if (state == PlayerState::Attack && m_frameNow <= 3)
    {
        pos.x += m_attackMomentum.x * dt;
        m_attackMomentum.x *= 0.9f;  // Decay momentum
    }
    else
    {
        m_attackMomentum.x = 0.f;  // Reset not attacking
    }

    if (m_comboTimer > 0.f)
    {
        m_comboTimer -= dt;
        if (m_comboTimer <= 0.f)
        {
            // Combo expired
            m_comboCount = 0;
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


    if (state == PlayerState::Attack && m_frameNow >= 3 && m_frameNow <= 5)
    {
        if (!m_hasHitThisAttack)
        {
            canDamageEnemy = true;
        }
    }
    else
    {
        canDamageEnemy = false;
    }

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

    m_comboText.emplace(font, "", 24);
    m_comboText->setFillColor(sf::Color(255, 165, 0));  // Orange
    m_comboText->setOutlineColor(sf::Color::Black);
    m_comboText->setOutlineThickness(2.f);
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
}

void player::RenderBPMVisualsAtPosition(sf::RenderWindow& window, const sf::Vector2f& screenPos)
{

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

    if (m_comboCount > 0 && m_comboText.has_value()) // Not working right now 
    {
        sf::FloatRect comboBounds = m_comboText->getLocalBounds();
        m_comboText->setPosition({
            screenPos.x - comboBounds.size.x / 2.f,
            screenPos.y - 150.f  // Above timing feedback
            });
        window.draw(*m_comboText);
    }
}

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