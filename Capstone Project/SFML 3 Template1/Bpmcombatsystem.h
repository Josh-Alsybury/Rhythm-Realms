#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

class BPMCombatSystem
{
public:
    enum class HitTiming
    {
        Perfect,
        Good,
        Okay,
        Miss
    };

    struct BeatInfo
    {
        float currentPhase;
        float timeUntilNextBeat;
        float timeSinceLastBeat;
        bool isOnBeat;
    };

private:
    float m_bpm;
    float m_beatPhase;
    float m_lastBeatTime;
    float m_gameTime;

    const float PERFECT_WINDOW = 0.05f;
    const float GOOD_WINDOW = 0.15f;
    const float OKAY_WINDOW = 0.25f;

    sf::CircleShape m_beatIndicator;
    sf::RectangleShape m_timingBar;
    sf::RectangleShape m_perfectZone;
    sf::RectangleShape m_goodZone;
    sf::Font* m_font;
    sf::Text m_timingText;

    float m_beatFlashTimer;
    const float BEAT_FLASH_DURATION = 0.1f;

    int m_currentCombo;
    int m_maxCombo;
    float m_comboMultiplier;
    float m_comboTimer;
    const float COMBO_TIMEOUT = 2.0f;

public:
    BPMCombatSystem(sf::Font& font)
        : m_bpm(120.0f)
        , m_beatPhase(0.0f)
        , m_lastBeatTime(0.0f)
        , m_gameTime(0.0f)
        , m_beatFlashTimer(0.0f)
        , m_currentCombo(0)
        , m_maxCombo(0)
        , m_comboMultiplier(1.0f)
        , m_comboTimer(0.0f)
        , m_font(&font)
        , m_timingText(font)
    {
        setupVisuals();
    }

    void setupVisuals()
    {
        m_beatIndicator.setRadius(20.f);
        m_beatIndicator.setFillColor(sf::Color(255, 255, 255, 100));
        m_beatIndicator.setOrigin({ 20.f, 20.f });
        m_beatIndicator.setPosition({ 100.f, 150.f });

        m_timingBar.setSize({ 300.f, 40.f });
        m_timingBar.setFillColor(sf::Color(50, 50, 50, 200));
        m_timingBar.setPosition({ 50.f, 200.f });
        m_timingBar.setOutlineColor(sf::Color::White);
        m_timingBar.setOutlineThickness(2.f);

        m_perfectZone.setSize({ 30.f, 40.f });
        m_perfectZone.setFillColor(sf::Color(0, 255, 0, 150));
        m_perfectZone.setPosition({ 185.f, 200.f });

        m_goodZone.setSize({ 100.f, 40.f });
        m_goodZone.setFillColor(sf::Color(255, 255, 0, 100));
        m_goodZone.setPosition({ 150.f, 200.f });
    }

    void setFont(sf::Font& font)
    {
        m_font = &font;
        m_timingText.setFont(font);
        m_timingText.setCharacterSize(24);
        m_timingText.setFillColor(sf::Color::White);
        m_timingText.setPosition({ 50.f, 250.f });
    }

    void setBPM(float bpm)
    {
        if (bpm > 0.0f)
            m_bpm = bpm;
    }

    float getBPM() const { return m_bpm; }


    void update(float dt)
    {
        m_gameTime += dt;

        float beatPeriod = 60.0f / m_bpm;
        m_beatPhase += dt / beatPeriod;

        if (m_beatPhase >= 1.0f)
        {
            m_beatPhase -= std::floor(m_beatPhase);
            m_lastBeatTime = m_gameTime;
            m_beatFlashTimer = BEAT_FLASH_DURATION;

            std::cout << "BEAT at " << m_gameTime << "s" << std::endl;
        }
        if (m_beatFlashTimer > 0.0f)
            m_beatFlashTimer -= dt;

        if (m_currentCombo > 0)
        {
            m_comboTimer += dt;
            if (m_comboTimer > COMBO_TIMEOUT)
            {
                resetCombo();
            }
        }
        updateVisuals();
    }

    void updateVisuals()
    {
        float pulseScale = 1.0f;
        if (m_beatFlashTimer > 0.0f)
        {
            float t = m_beatFlashTimer / BEAT_FLASH_DURATION;
            pulseScale = 1.0f + t * 0.5f;
        }

        m_beatIndicator.setScale({ pulseScale, pulseScale });

        float glow = std::sin(m_beatPhase * 3.14159f * 2.0f) * 0.5f + 0.5f;
        m_beatIndicator.setFillColor(sf::Color(
            255,
            static_cast<std::uint8_t>(255 * glow),
            static_cast<std::uint8_t>(255 * glow),
            200
        ));

        if (m_font)
        {
            std::string text = "Combo: " + std::to_string(m_currentCombo);
            text += "\nMultiplier: x" + std::to_string(m_comboMultiplier).substr(0, 4);
            m_timingText.setString(text);
        }


    }

    BeatInfo getCurrentBeatInfo() const
    {
        BeatInfo info;
        info.currentPhase = m_beatPhase;

        float beatPeriod = 60.0f / m_bpm;
        info.timeUntilNextBeat = (1.0f - m_beatPhase) * beatPeriod;
        info.timeSinceLastBeat = m_beatPhase * beatPeriod;

        float distanceFromBeat = std::min(info.timeSinceLastBeat, info.timeUntilNextBeat);
        info.isOnBeat = (distanceFromBeat <= PERFECT_WINDOW);

        return info;
    }
    HitTiming evaluateHitTiming() const
    {
        BeatInfo beat = getCurrentBeatInfo();
        float distanceFromBeat = std::min(beat.timeSinceLastBeat, beat.timeUntilNextBeat);

        if (distanceFromBeat <= PERFECT_WINDOW)
            return HitTiming::Perfect;
        else if (distanceFromBeat <= GOOD_WINDOW)
            return HitTiming::Good;
        else if (distanceFromBeat <= OKAY_WINDOW)
            return HitTiming::Okay;
        else
            return HitTiming::Miss;

    }

    float registerHit(HitTiming timing)
    {

        float damageMultiplier = 1.0f;
        switch (timing)
        {
        case HitTiming::Perfect:
            damageMultiplier = 2.0f;
            m_currentCombo++;
            m_comboTimer = 0.0f;
            std::cout << "PERFECT HIT!  Combo: " << m_currentCombo << std::endl;
            break;

        case HitTiming::Good:
            damageMultiplier = 1.5f;
            m_currentCombo++;
            m_comboTimer = 0.0f;
            std::cout << "Good Hit! Combo: " << m_currentCombo << std::endl;
            break;

        case HitTiming::Okay:
            damageMultiplier = 1.0f;
            m_comboTimer = 0.0f;
            std::cout << "Okay hit." << std::endl;
            break;

        case HitTiming::Miss:
            damageMultiplier = 0.5f;
            resetCombo();
            std::cout << "Missed timing!" << std::endl;
            break;
        }

        m_comboMultiplier = 1.0f + std::min(m_currentCombo * 0.1f, 1.0f);

        if (m_currentCombo > m_maxCombo)
            m_maxCombo = m_currentCombo;


        return damageMultiplier * m_comboMultiplier;
    }

    float registerBlock(HitTiming timing)
    {
        float blockEffectiveness = 0.5f;

        switch (timing)
        {

        case HitTiming::Perfect:
            blockEffectiveness = 1.0f;
            m_currentCombo++;
            m_comboTimer = 0.0f;
            std::cout << "PERFECT PARRY! " << std::endl;
            return -1.0f;

        case HitTiming::Good:
            blockEffectiveness = 0.8f;
            m_comboTimer = 0.0f;
            std::cout << "Good Block!" << std::endl;
            break;

        case HitTiming::Okay:
            blockEffectiveness = 0.5f;
            m_comboTimer = 0.0f;
            std::cout << "Block." << std::endl;
            break;

        case HitTiming::Miss:
            blockEffectiveness = 0.2f;
            resetCombo();
            std::cout << "Weak block!" << std::endl;
            break;
        }
        return blockEffectiveness;
    }

    void resetCombo()
    {

        if (m_currentCombo > 0)
        {
            std::cout << "Combo broken! Final: " << m_currentCombo << std::endl;
        }

        m_currentCombo = 0;

        m_comboMultiplier = 1.0f;

        m_comboTimer = 0.0f;
    }

    int getCurrentCombo() const { return m_currentCombo; }
    int getMaxCombo() const { return m_maxCombo; }

    float getComboMultiplier() const { return m_comboMultiplier; }


    void render(sf::RenderWindow& window)
    {
        window.draw(m_timingBar);
        window.draw(m_goodZone);
        window.draw(m_perfectZone);

        float barWidth = m_timingBar.getSize().x;
        float indicatorX = m_timingBar.getPosition().x + (m_beatPhase * barWidth);

        sf::RectangleShape cursor({ 4.f, 50.f });
        cursor.setFillColor(sf::Color::Cyan);
        cursor.setPosition({ indicatorX, 195.f });
        window.draw(cursor);

        window.draw(m_beatIndicator);

        if (m_font)
            window.draw(m_timingText);
    }

    sf::Color getTimingColor() const
    {

        HitTiming timing = evaluateHitTiming();
        switch (timing)
        {
        case HitTiming::Perfect: return sf::Color::Green;
        case HitTiming::Good:    return sf::Color::Yellow;
        case HitTiming::Okay:    return sf::Color(255, 165, 0);
        case HitTiming::Miss:    return sf::Color::Red;
        }
        return sf::Color::White;

    }

    bool isInGoodTimingWindow() const
    {
        BeatInfo beat = getCurrentBeatInfo();
        float distanceFromBeat = std::min(beat.timeSinceLastBeat, beat.timeUntilNextBeat);
        return distanceFromBeat <= GOOD_WINDOW;

    }

};