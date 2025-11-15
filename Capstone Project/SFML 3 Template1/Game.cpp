/// <summary>
/// author Pete Lowe May 2025
/// you need to change the above line or lose marks
/// </summary>
#include "Headers/Game.h"
#include <iostream>


/// <summary>
/// default constructor
/// setup the window properties
/// load and setup the texts
/// load and setup the images
/// load and setup the sounds
/// </summary>
Game::Game() :
	m_window{ sf::VideoMode{ sf::Vector2u{1000U, 800U}, 32U }, "SFML Game 3.0" },
	m_DELETEexitGame{false} //when true game will exit
{
	m_dynamicBackground.loadtheme("ASSETS/IMAGES/Autumn Forest 2D Pixel Art/Background");

	m_Player.pos.x = m_window.getSize().x / 2.f;
	setupSprites(); // load texture
	setupTexts();   // load font
	m_Player.SetupPlayer();
	m_Player.HealCall();
	m_Player.Health();

	m_enemies.reserve(2);  // Pre-allocate for exactly 2

	// Initialize both enemies
	for (int i = 0; i < 2; ++i)
	{
		m_enemies.emplace_back(); // adds a new Enemy1 to the vector
		m_enemies.back().SetupEnemy1(); // setup the last added enemy
		m_enemies.back().pos = { 850.f + (i * 300.f), 780.f };
		m_enemies.back().sprite->setPosition(m_enemies.back().pos);
	}

	setupAudio();
}

/// <summary>
/// default destructor we didn't dynamically allocate anything
/// so we don't need to free it, but mthod needs to be here
/// </summary>
Game::~Game()
{
}


/// <summary>
/// main game loop
/// update 60 times per second,
/// process update as often as possible and at least 60 times per second
/// draw as often as possible but only updates are on time
/// if updates run slow then don't render frames
/// </summary>
void Game::run()
{	
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	const float fps{ 60.0f };
	sf::Time timePerFrame = sf::seconds(1.0f / fps);
	while (m_window.isOpen())
	{
		processEvents(); 
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); 
			update(timePerFrame); /
		}
		render(); 
	}
}
/// <summary>
/// handle user and system events/ input
/// get key presses/ mouse moves etc. from OS
/// and user :: Don't do game update here
/// </summary>
void Game::processEvents()
{
	
	while (const std::optional newEvent = m_window.pollEvent())
	{
		if ( newEvent->is<sf::Event::Closed>()) // close window message 
		{
			m_DELETEexitGame = true;
		}
		if (newEvent->is<sf::Event::KeyPressed>()) 
		{
			processKeys(newEvent);
		}

		// Handle mouse clicks
		if (newEvent->is<sf::Event::MouseButtonPressed>())
		{
			if (m_showSkillTree)
			{
				const auto* mousePress = newEvent->getIf<sf::Event::MouseButtonPressed>();
				if (mousePress->button == sf::Mouse::Button::Left)
				{
					sf::Vector2f mousePos = m_window.mapPixelToCoords(mousePress->position);
					m_skillTree.HandleClick(mousePos);
				}
			}
		}

		// Handle mouse movement for hover tooltips
		if (newEvent->is<sf::Event::MouseMoved>())
		{
			if (m_showSkillTree)
			{
				const auto* mouseMove = newEvent->getIf<sf::Event::MouseMoved>();
				sf::Vector2f mousePos = m_window.mapPixelToCoords(mouseMove->position);
				m_skillTree.UpdateHover(mousePos);
			}
		}
	}
	
}


/// <summary>
/// deal with key presses from the user
/// </summary>
/// <param name="t_event">key press event</param>
void Game::processKeys(const std::optional<sf::Event> t_event)
{
	const sf::Event::KeyPressed *newKeypress = t_event->getIf<sf::Event::KeyPressed>();
	if (sf::Keyboard::Key::Escape == newKeypress->code)
	{
		m_DELETEexitGame = true; 
	}
	if (sf::Keyboard::Key::N == newKeypress->code)
	{
		switchSong();
	}
	if (sf::Keyboard::Key::T == newKeypress->code)
	{
		m_showSkillTree = !m_showSkillTree; 
	}
}

/// <summary>
/// Check if any keys are currently pressed
/// </summary>
void Game::checkKeyboardState()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
	{
		m_DELETEexitGame = true; 
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P))
	{
		m_Player.TakeDamage(1);
	}
}

/// <summary>
/// Update the game world
/// </summary>
/// <param name="t_deltaTime">time interval per frame</param>
/// <summary>
/// Update the game world
/// </summary>
/// <param name="t_deltaTime">time interval per frame</param>
void Game::update(sf::Time t_deltaTime)
{
	checkKeyboardState();

	m_currentBPM = m_bpmStream.getCurrentBPM();

	if (m_currentBPM > 0.0)
	{
		if (m_currentBPM < 90.0 && m_currentTheme != "Medieval")
		{
			std::cout << "Transitioning to Medieval theme (BPM: " << m_currentBPM << ")" << std::endl;
			m_dynamicBackground.transitionTo("ASSETS/IMAGES/Background");
			m_currentTheme = "Medieval";
		}
		else if (m_currentBPM >= 90.0 && m_currentBPM <= 150.0 && m_currentTheme != "Forest")
		{
			std::cout << "Transitioning to Forest theme (BPM: " << m_currentBPM << ")" << std::endl;
			m_dynamicBackground.transitionTo("ASSETS/IMAGES/Autumn Forest 2D Pixel Art/Background");
			m_currentTheme = "Forest";
		}
	}
	m_bpmText.setString("Live BPM: " + std::to_string(static_cast<int>(m_currentBPM)));

	if (!m_showSkillTree)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
		{
			m_Player.moveLeft();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
		{
			m_Player.moveRight();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
		{
			m_Player.Jump();
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
		{
			m_Player.Attack();
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			m_Player.Defend();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
		{
			m_Player.Heal();
		}
	}

	float leftMargin = m_screenMargin;
	float rightMargin = m_window.getSize().x - m_screenMargin;

	float playerScreenX = m_Player.pos.x - m_cameraOffset.x;

	if (playerScreenX > rightMargin)
		m_cameraOffset.x += playerScreenX - rightMargin;
	else if (playerScreenX < leftMargin)
		m_cameraOffset.x -= leftMargin - playerScreenX;

	m_dynamicBackground.update(m_cameraOffset);
	float dt = t_deltaTime.asSeconds();
	m_Player.Update(dt);

	if (m_DELETEexitGame)
	{
		m_window.close();
	}

	// QOL: Pause enemies when skill tree is open
	if (!m_showSkillTree)
	{
		for (auto& enemy : m_enemies)
		{
			enemy.Update(dt, m_Player.pos);

			// Check player attacking enemy
			if (m_Player.canDamageEnemy)
			{
				float dx = m_Player.attackHitbox.getPosition().x - enemy.pos.x;
				float dy = m_Player.attackHitbox.getPosition().y - enemy.pos.y;
				float distance = std::sqrt(dx * dx + dy * dy);

				if (distance < m_Player.attackHitboxRadius + 30.f)
				{
					enemy.TakeDamage(1);
					m_Player.canDamageEnemy = false;

					if (enemy.health <= 0)
					{
						std::cout << "Enemy defeated! Recycling..." << std::endl;
						m_skillTree.AddSkillPoint();

						float furthestX = 0.f;
						for (const auto& otherEnemy : m_enemies)
						{
							if (otherEnemy.pos.x > furthestX && otherEnemy.health > 0)
								furthestX = otherEnemy.pos.x;
						}

						// Reset the defeated enemy
						enemy.health = Enemy1::MAX_HEALTH;
						enemy.pos = { furthestX + 300.f, 780.f };
						enemy.sprite->setPosition(enemy.pos);
						enemy.UpdateHealthBar();
						enemy.state = Enemy1::EnemyState::Idle;
						enemy.attackCooldown = 0.f;
						enemy.hasDealtDamage = false;
						enemy.canDamagePlayer = false;
					}
				}
			}

			// Check enemy attacking player
			if (enemy.canDamagePlayer && !enemy.hasDealtDamage)
			{
				float dx = enemy.attackHitbox.getPosition().x - m_Player.pos.x;
				float dy = enemy.attackHitbox.getPosition().y - m_Player.pos.y;
				float distance = std::sqrt(dx * dx + dy * dy);

				if (m_Player.canBlockEnemy)
				{
					std::cout << "Attack blocked!" << std::endl;
					float knockbackDirection = (m_Player.pos.x > enemy.pos.x) ? 1.0f : -1.0f;

					m_Player.velocity.x = knockbackDirection * 500.f;
					if (m_Player.isOnGround)
						m_Player.velocity.y = -10.f;

					m_Player.isKnockedBack = true;
					m_Player.knockbackTimer = m_Player.KNOCKBACK_DURATION;

					enemy.hasDealtDamage = true;
					enemy.canDamagePlayer = false;
				}
				else
				{
					m_Player.TakeDamage(1);
					enemy.hasDealtDamage = true;
					enemy.canDamagePlayer = false;
					std::cout << "Player hit! Health: " << m_Player.health << std::endl;
				}
			}
		}
	}
}

void Game::switchSong()
{
	m_currentSongIndex = (m_currentSongIndex + 1) % m_songPaths.size();

	m_bpmStream.stop();

	std::cout << "Switching to song " << m_currentSongIndex + 1 << "..." << std::endl;

	if (m_bpmStream.load(m_songPaths[m_currentSongIndex]))
	{
		m_bpmStream.play();
		std::cout << "Song loaded successfully!" << std::endl;
	}
	else
	{
		std::cerr << "Failed to load song!" << std::endl;
	}
}

/// <summary>
/// draw the frame and then switch buffers
/// </summary>
/// <summary>
/// draw the frame and then switch buffers
/// </summary>
void Game::render()
{
	m_window.clear(sf::Color::Blue);

	sf::Vector2f renderPos = m_Player.pos - m_cameraOffset;
	m_Player.sprite->setPosition(renderPos);
	m_dynamicBackground.render(m_window);

	// Draw game world first
	for (auto& enemy : m_enemies)
	{
		sf::Vector2f enemyRenderPos = enemy.pos - m_cameraOffset;
		enemy.sprite->setPosition(enemyRenderPos);
		enemy.detectionRadius.setPosition(enemyRenderPos);
		enemy.attackRadius.setPosition(enemyRenderPos);

		m_window.draw(enemy.detectionRadius);
		m_window.draw(enemy.attackRadius);
		m_window.draw(*enemy.sprite);

		for (auto& bar : enemy.healthBar)
		{
			sf::RectangleShape offsetBar = bar;
			offsetBar.setPosition(bar.getPosition() - m_cameraOffset);
			m_window.draw(offsetBar);
		}
	}

	m_window.draw(*m_Player.sprite);
	m_window.draw(m_bpmText);

	// Draw UI elements (always on top)
	for (int i = 0; i < m_Player.HealsCount; i++)
	{
		m_window.draw(m_Player.HealSphere[i]);
	}

	for (auto& bar : m_Player.HealBar)
	{
		m_window.draw(bar);
	}

	if (m_showSkillTree)
	{
		// QOL: Add semi-transparent dark overlay
		sf::RectangleShape overlay(sf::Vector2f(m_window.getSize()));
		overlay.setFillColor(sf::Color(0, 0, 0, 180));  // Dark transparent background
		m_window.draw(overlay);

		m_skillTree.Draw(m_window);
	}

	m_window.display();
}

/// <summary>
/// load the font and setup the text message for screen
/// </summary>
void Game::setupTexts()
{
	if (!m_jerseyFont.openFromFile("ASSETS/FONTS/Jersey20-Regular.ttf"))
	{
		std::cout << "Error loading font!\n";
	}
}


/// <summary>
/// load the texture and setup the sprite for the logo
/// </summary>
void Game::setupSprites()
{

}

/// <summary>
/// load sound file and assign buffers
/// </summary>
void Game::setupAudio()
{
	m_songPaths = {
		"ASSETS/AUDIO/ThemeofAmaterasu.wav",  
		"ASSETS/AUDIO/Moorland.wav",                 
		"ASSETS/AUDIO/Starjunk95OceanMemory.wav"                
	};
	m_currentSongIndex = 0;

	std::cout << "Loading audio file..." << std::endl;

	if (!m_bpmStream.load(m_songPaths[m_currentSongIndex]))
	{
		std::cerr << "Failed to load initial audio file!" << std::endl;
		return;
	}

	std::cout << "Audio loaded successfully, starting playback..." << std::endl;
	m_bpmStream.setVolume(0.0f);
	m_bpmStream.play();

	m_bpmText.setCharacterSize(32);
	m_bpmText.setFillColor(sf::Color::White);
	m_bpmText.setPosition(sf::Vector2f(50.f, 50.f));
	m_bpmText.setString("Live BPM: 0.0");
}


