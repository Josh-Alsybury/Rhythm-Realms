/// <summary>
/// author Pete Lowe May 2025
/// you need to change the above line or lose marks
/// </summary>
#include "Headers/Game.h"
#include <iostream>
#include <cstdlib> 

/// <summary>
/// default constructor
/// setup the window properties
/// load and setup the texts
/// load and setup the images
/// load and setup the sounds
/// </summary>
Game::Game() :
	m_window{ sf::VideoMode{ {1000, 800}, 32 }, "SFML Game 3.0" },
	m_DELETEexitGame{ false },
	m_gameTimer(0.f)
{
	setupTexts();
	m_mainMenu = std::make_unique<Menu>(m_jerseyFont);
	m_Player.SetupPlayer();
	m_showMenu = true;
	m_useSpotify = false;

	m_gameView = m_window.getDefaultView();

	if (!g_samuraiTextures.load() || !g_archerTextures.load())
	{
		throw std::runtime_error("Failed to load enemy textures");
	}

	m_easyConfig = EnemySpawnConfig(800.f, 1200.f, 5.0f, 2);
	m_normalConfig = EnemySpawnConfig(800.f, 1200.f, 3.0f, 3);
	m_hardConfig = EnemySpawnConfig(800.f, 1200.f, 1.5f, 5);
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
			update(timePerFrame);
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
		// Window close
		if (newEvent->is<sf::Event::Closed>())
		{
			m_DELETEexitGame = true;
		}

		if (m_isInHub && m_hub.IsShopOpen())
		{
			m_hub.GetShopUI().handleEvent(*newEvent);
			// Optionally still allow Esc to quit:
			if (const auto* key = newEvent->getIf<sf::Event::KeyPressed>())
			{
				if (key->code == sf::Keyboard::Key::Escape)
					m_DELETEexitGame = true;
			}
			continue; // skip normal game/menu handling
		}

		// Keyboard
		if (newEvent->is<sf::Event::KeyPressed>())
		{
			processKeys(newEvent);
		}

		// Mouse button pressed
		if (newEvent->is<sf::Event::MouseButtonPressed>())
		{
			const auto* mousePress = newEvent->getIf<sf::Event::MouseButtonPressed>();

			if (mousePress->button == sf::Mouse::Button::Left)
			{
				sf::Vector2f mousePos =
					m_window.mapPixelToCoords(mousePress->position);

				// ---- MENU CLICK HANDLING ----
				if (m_showMenu)
				{
					m_mainMenu->HandleClick(mousePos);

					if (m_mainMenu->IsComplete())
					{
						m_selectedAudioSource =
							m_mainMenu->GetSelectedSource();

						m_showMenu = false;
						if (m_selectedAudioSource == Menu::AudioSource::Spotify)
						{
							m_useSpotify = true;

							// Start Spotify API for track info
							std::cout << "Starting Spotify server..." << std::endl;
							#ifdef _WIN32
							system("start cmd /k \"..\\..\\Spotify test\\start_spotify_server.bat\"");
							#endif
							std::this_thread::sleep_for(std::chrono::seconds(3));
							m_spotifyClient.StartPolling();

						}
						initializeGame(); //  moved out of constructor
					}
				}
				// ---- SKILL TREE CLICK ----
				else if (m_showSkillTree)
				{
					m_skillTree.HandleClick(mousePos);
				}
			}
		}
		// Mouse movement (hover tooltips)
		if (newEvent->is<sf::Event::MouseMoved>())
		{
			if (m_showSkillTree)
			{
				const auto* mouseMove =
					newEvent->getIf<sf::Event::MouseMoved>();

				sf::Vector2f mousePos =
					m_window.mapPixelToCoords(mouseMove->position);

				m_skillTree.UpdateHover(mousePos);
			}
		}
	}
}

void Game::initializeGame()
{
	m_bpmText.setFont(m_jerseyFont);
	m_bpmText.setCharacterSize(32);
	m_bpmText.setFillColor(sf::Color::White);
	m_bpmText.setPosition(sf::Vector2f(10.f, 75.f));
	m_bpmText.setString("BPM: 0");

	auto size = m_window.getSize();
	float totalThickness = 64.f;          // full width of the effect
	float sliceThickness = totalThickness / BORDER_SLICES;

	for (int i = 0; i < BORDER_SLICES; ++i)
	{
		float inset = i * sliceThickness;

		// Top
		m_screenBorders[0][i].setSize({ static_cast<float>(size.x), sliceThickness });
		m_screenBorders[0][i].setPosition({ 0.f, inset });

		// Bottom
		m_screenBorders[1][i].setSize({ static_cast<float>(size.x), sliceThickness });
		m_screenBorders[1][i].setPosition({ 0.f, size.y - inset - sliceThickness });

		// Left
		m_screenBorders[2][i].setSize({ sliceThickness, static_cast<float>(size.y) });
		m_screenBorders[2][i].setPosition({ inset, 0.f });

		// Right
		m_screenBorders[3][i].setSize({ sliceThickness, static_cast<float>(size.y) });
		m_screenBorders[3][i].setPosition({ size.x - inset - sliceThickness, 0.f });

		for (int e = 0; e < 4; ++e)
			m_screenBorders[e][i].setFillColor(sf::Color::Transparent);
	}

	// Audio setup
	if (!m_useSpotify)
	{
		setupAudio();
	}
	else
	{
		std::cout << "Using Spotify BPM source" << std::endl;
	}

	// Player setup
	m_Player.SetupPlayer();
	m_Player.HealCall();
	m_Player.Health();

	m_gameTimer = 0.f;

	// ===== START IN HUB =====
	m_isInHub = true;
	m_currentGameTheme = GameTheme::Hub;

	// Load HUB tileset
	std::string tilesetPath = DynamicBackground::GetTilesetTexturePath(m_currentGameTheme);
	if (!m_tilesetTexture.loadFromFile(tilesetPath))
	{
		std::cout << "Failed to load hub tileset: " << tilesetPath << std::endl;
	}
	m_tilesetTexture.setSmooth(false);

	// Clear enemies (no enemies in hub)
	m_enemies.clear();
	m_archers.clear();
	m_arrows.clear();

	// USE HUB CLASS TO LOAD
	m_hub.Load(m_tilesetTexture, m_chunks, m_Player, m_chunkWidth, m_jerseyFont, m_window.getSize());

	if (!m_useSpotify)
	{
		m_bpmStream.setVolume(0.f);
	}
}


/// <summary>
/// deal with key presses from the user
/// </summary>
/// <param name="t_event">key press event</param>
void Game::processKeys(const std::optional<sf::Event> t_event)
{
	const sf::Event::KeyPressed* newKeypress = t_event->getIf<sf::Event::KeyPressed>();
	if (sf::Keyboard::Key::Escape == newKeypress->code)
	{
		m_DELETEexitGame = true;
	}
	if (sf::Keyboard::Key::N == newKeypress->code)
	{
		if (m_useSpotify)
			return;

		switchSong();
	}
	if (sf::Keyboard::Key::T == newKeypress->code)
	{
		m_showSkillTree = !m_showSkillTree;
	}
	if (sf::Keyboard::Key::F3 == newKeypress->code)
	{
		m_showDebugCollision = !m_showDebugCollision;
		std::cout << "Debug collision: " << (m_showDebugCollision ? "ON" : "OFF") << std::endl;
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
void Game::update(sf::Time t_deltaTime)
{
	checkKeyboardState();
	float dt = t_deltaTime.asSeconds();

	// ===== PLAYER INPUT (always active) =====
	if (!m_showSkillTree && !(m_isInHub && m_hub.IsShopOpen()))
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
			m_Player.moveLeft();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
			m_Player.moveRight();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
			m_Player.Jump();
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
			m_Player.Attack();
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			m_Player.Defend();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
			m_Player.Heal();
	}

	if (m_isInHub)
	{
		float hubWidth = 0.f;
		for (auto& chunk : m_chunks)
		{
			hubWidth += chunk.getWidth();
		}

		m_Player.health = m_Player.MAX_HEALTH;

		float viewWidth = m_window.getSize().x;
		float viewHeight = m_window.getSize().y;

		// Clamp to hub boundaries
		float minCameraX = 0.f;
		float rightPadding = 450.f; // reduce the right side
		float maxCameraX = std::max(0.f, hubWidth - viewWidth - rightPadding);

		// Center camera on player with smooth following
		float targetCameraX = m_Player.pos.x - viewWidth / 1.5f;
		m_cameraOffset.x += (targetCameraX - m_cameraOffset.x) * dt;

		m_cameraOffset.x = std::clamp(m_cameraOffset.x, minCameraX, maxCameraX);

		// Apply view (no zoom, just positioning)
		m_gameView.setSize({ viewWidth, viewHeight });
		m_gameView.setCenter({ m_cameraOffset.x + viewWidth / 2.f,
			m_cameraOffset.y + viewHeight / 2.f });
	}
	else
	{
		float leftMargin = m_screenMargin;
		float rightMargin = (m_window.getSize().x - 150) - m_screenMargin;
		float playerScreenX = m_Player.pos.x - m_cameraOffset.x;

		if (playerScreenX > rightMargin)
			m_cameraOffset.x += playerScreenX - rightMargin;
		else if (playerScreenX < leftMargin)
			m_cameraOffset.x -= leftMargin - playerScreenX;

		m_cameraOffset.x = std::round(m_cameraOffset.x);
		m_cameraOffset.y = std::round(m_cameraOffset.y);
	}

	// ===== PLAYER COLLISION (always active) =====
	sf::Vector2f oldPos = m_Player.pos;
	m_Player.pos.x += m_Player.velocity.x * dt;

	sf::FloatRect playerBox(
		{ m_Player.pos.x - PLAYER_HITBOX_WIDTH / 2.f, m_Player.pos.y - PLAYER_HITBOX_HEIGHT / 10.f },
		{ PLAYER_HITBOX_WIDTH, PLAYER_HITBOX_HEIGHT }
	);

	bool hitWall = false;
	for (auto& chunk : m_chunks)
	{
		if (chunk.isSolidTileWorld(playerBox.position.x, playerBox.position.y) ||
			chunk.isSolidTileWorld(playerBox.position.x + playerBox.size.x, playerBox.position.y) ||
			chunk.isSolidTileWorld(playerBox.position.x, playerBox.position.y + playerBox.size.y) ||
			chunk.isSolidTileWorld(playerBox.position.x + playerBox.size.x, playerBox.position.y + playerBox.size.y))
		{
			hitWall = true;
			break;
		}
	}

	if (hitWall)
	{
		m_Player.pos.x = oldPos.x;
		m_Player.velocity.x = 0;
	}

	m_Player.isOnGround = false;
	float playerFeetY = m_Player.pos.y + 50.0f;

	for (auto& chunk : m_chunks)
	{
		if (chunk.isSolidTileWorld(m_Player.pos.x, playerFeetY))
		{
			if (m_Player.velocity.y >= 0)
			{
				m_Player.isOnGround = true;
				m_Player.velocity.y = 0;

				float chunkRelativeY = playerFeetY - chunk.getPosition().y;
				float tileSize = 32.f; // or chunk.getTileSize()
				int tileY = static_cast<int>(chunkRelativeY / tileSize);
				m_Player.pos.y = chunk.getPosition().y + tileY * tileSize - 50.f;
				break;
			}
		}
	}

	m_Player.Update(dt);

	if (m_DELETEexitGame)
	{
		m_window.close();
	}

	// ===== STATE-BASED UPDATE =====
	if (m_isInHub)
	{
		// USE HUB CLASS TO UPDATE
		if (m_hub.Update(dt, m_Player))
		{
			std::cout << "\n=== STARTING EXPEDITION ===" << std::endl;

			m_isInHub = false;
			m_currentGameTheme = GameTheme::Forest;

			if (!m_useSpotify)
			{
				m_bpmStream.setVolume(50.f); // your normal level
			}

			// Load expedition background
			m_dynamicBackground.setCurrentTheme(m_currentGameTheme);
			m_dynamicBackground.loadtheme(DynamicBackground::GetBackgroundPath(m_currentGameTheme));

			// Load expedition tileset
			std::string tilesetPath = DynamicBackground::GetTilesetTexturePath(m_currentGameTheme);
			if (m_tilesetTexture.loadFromFile(tilesetPath))
			{
				m_tilesetTexture.setSmooth(false);
			}

			// Load expedition chunks
			m_chunks.clear();
			m_chunks.resize(VISIBLE_CHUNKS);
			loadChunkAt(0, 0);
			m_chunkWidth = m_chunks[0].getWidth();
			for (int i = 1; i < VISIBLE_CHUNKS; ++i)
			{
				loadChunkAt(i, i * m_chunkWidth);
			}

			// Position player
			m_Player.pos.x = 500.f;
			m_Player.pos.y = 500.f;

			// Spawn enemies
			m_enemySpawnManager.ForceSpawn({ 850.f, 746.f }, m_enemies);
			m_enemySpawnManager.ForceSpawn({ 1150.f, 746.f }, m_enemies);
			m_enemySpawnManager.ForceSpawnArcher({ 1500.f, 746.f }, m_archers);

			std::cout << "Expedition started!" << std::endl;
		}
	}
	else  // EXPEDITION MODE
	{
		// Update background
		m_dynamicBackground.update(m_cameraOffset);
		m_gameTimer += dt;

		// BPM processing
		float rawBPM = 0.f;
		if (m_useSpotify)
		{
			auto trackInfo = m_spotifyClient.GetCurrentTrack();
			rawBPM = trackInfo.bpm;
		}
		else
		{
			rawBPM = m_bpmStream.getCurrentBPM();
		}

		if (rawBPM <= 0.f)
			rawBPM = m_currentBPM;

		static float smoothedBPM = 120.f;
		smoothedBPM += (rawBPM - smoothedBPM) * 0.05f;
		m_currentBPM = smoothedBPM;
		float bpmForTheme = rawBPM;

		// BPM text
		if (m_useSpotify)
			m_bpmText.setString("Track Tempo: " + std::to_string((int)m_currentBPM));
		else
			m_bpmText.setString("Live BPM: " + std::to_string((int)m_currentBPM));

		// Theme switching
		if (bpmForTheme > 0.0)
		{
			GameTheme newTheme = DynamicBackground::GetThemeFromBPM(bpmForTheme);

			if (newTheme != m_currentGameTheme)
			{
				std::cout << "BPM " << bpmForTheme << " -> Switching to theme: " << (int)newTheme << std::endl;

				m_currentGameTheme = newTheme;
				m_dynamicBackground.setCurrentTheme(newTheme);
				m_dynamicBackground.transitionTo(DynamicBackground::GetBackgroundPath(newTheme));

				std::string newTilesetPath = DynamicBackground::GetTilesetTexturePath(newTheme);
				if (m_tilesetTexture.loadFromFile(newTilesetPath))
				{
					m_tilesetTexture.setSmooth(false);
					std::cout << "Loaded new tileset: " << newTilesetPath << std::endl;

					for (int i = 0; i < m_chunks.size(); ++i)
					{
						float chunkX = m_chunks[i].getPosition().x;
						loadChunkAt(i, chunkX);
					}
				}
			}
		}

		if (m_Player.health <= 0)
		{
			// Switch state
			m_isInHub = true;
			m_currentGameTheme = GameTheme::Hub;

			if (!m_useSpotify)
			{
				m_bpmStream.setVolume(0.f);
			}

			// Reset camera
			m_cameraOffset = { 0.f, 0.f };
			m_gameView = m_window.getDefaultView();

			// Clear expedition entities
			m_enemies.clear();
			m_archers.clear();
			m_arrows.clear();

			// Reload hub tileset
			std::string tilesetPath =
				DynamicBackground::GetTilesetTexturePath(GameTheme::Hub);
			m_tilesetTexture.loadFromFile(tilesetPath);
			m_tilesetTexture.setSmooth(false);

			// Reload hub chunks
			m_chunks.clear();
			m_hub.Load(m_tilesetTexture, m_chunks, m_Player, m_chunkWidth, m_jerseyFont, m_window.getSize());

			// Reset player safely
			m_Player.health = m_Player.MAX_HEALTH;
			m_Player.velocity = { 0.f, 0.f };
		}


		// Difficulty scaling
		if (m_currentBPM > 140.f)
			m_enemySpawnManager.SetSpawnConfig(m_hardConfig);
		else if (m_currentBPM < 100.f)
			m_enemySpawnManager.SetSpawnConfig(m_easyConfig);
		else
			m_enemySpawnManager.SetSpawnConfig(m_normalConfig);

		// Chunk scrolling
		updateChunks();

		if (!m_showSkillTree)
		{
			// Enemy spawning + updates (all your existing code)
			float rightmostChunkX = -999999.0f;
			for (const auto& chunk : m_chunks)
			{
				float chunkRight = chunk.getPosition().x + chunk.getWidth();
				if (chunkRight > rightmostChunkX)
					rightmostChunkX = chunkRight;
			}

			m_enemySpawnManager.Update(dt, m_Player.pos, m_enemies, m_archers, rightmostChunkX, m_chunks);
			float hpRatio = static_cast<float>(m_Player.health) / m_Player.MAX_HEALTH;
			hpRatio = std::clamp(hpRatio, 0.f, 1.f);

			// green -> yellow -> red
			sf::Color baseColor;
			if (hpRatio > 0.5f)
			{
				// green to yellow
				float t = (hpRatio - 0.5f) / 0.5f; // 0..1
				baseColor = sf::Color(
					static_cast<std::uint8_t>(255 * (1.f - t)),  // R: 0->255
					255,
					0);
			}
			else
			{
				// yellow to red
				float t = hpRatio / 0.5f; // 0..1
				baseColor = sf::Color(
					255,
					static_cast<std::uint8_t>(255 * t),          // G: 0->255
					0);
			}

			// ---- BPM-based flash ----
			// one cycle per beat
			float bpm = m_currentBPM <= 0.f ? 120.f : m_currentBPM;
			float beatPeriod = 60.f / bpm;          // seconds per beat
			m_bpmPhase += dt / beatPeriod;          // dt from earlier
			m_bpmPhase -= std::floor(m_bpmPhase);   // keep 0..1

			// simple pulse: bright at phase 0, dim at 0.5
			float pulse = std::sin(m_bpmPhase * 3.1415926f); // 0..1

			for (int edge = 0; edge < 4; ++edge)
			{
				for (int i = 0; i < BORDER_SLICES; ++i)
				{
					float t = static_cast<float>(i) / (BORDER_SLICES - 1); // 0 = outermost, 1 = innermost

					// Strong at screen edge, fades inward
					float alphaFactor = 1.f - t;          // 1 -> 0
					float pulseFactor = 0.4f + 0.6f * pulse; // keep some beat, but not too crazy

					std::uint8_t alpha = static_cast<std::uint8_t>(
						40 + 150 * alphaFactor * pulseFactor
						);

					sf::Color c = baseColor;
					c.a = alpha;
					m_screenBorders[edge][i].setFillColor(c);
				}
			}

			// ========== UPDATE MELEE ENEMIES ==========
			for (auto& enemy : m_enemies)
			{
				if (enemy.health <= 0) continue;

				// Store old position
				sf::Vector2f oldPos = enemy.pos;

				// Update enemy (AI + movement)
				enemy.Update(dt, m_Player.pos);
				// ledge coverage
				bool wouldFallOffLedge = EnemyCollision::IsLedgeAhead(
					oldPos,  // Check from OLD position
					enemy.facingRight,
					m_chunks,
					45.f
				);

				if (wouldFallOffLedge && enemy.velocity.x != 0.f)
				{
				
					enemy.pos.x = oldPos.x;

					// Back up slowly (half speed)
					float backupDir = enemy.facingRight ? -1.f : 1.f;
					enemy.velocity.x = backupDir * enemy.speed * 0.5f;
					enemy.pos.x += enemy.velocity.x * dt;

				}

				// Apply horizontal collision
				EnemyCollision::CheckHorizontalCollision(
					enemy.pos,
					enemy.velocity,
					m_chunks,
					dt
				);

				// Snap to ground (prevents floating)
				EnemyCollision::ApplyGravityAndGround(
					enemy.pos,
					enemy.velocity,  // Pass velocity so gravity can update it
					m_chunks,
					dt
				);

				// Update sprite position
				if (enemy.sprite)
					enemy.sprite->setPosition(enemy.pos);

				if (enemy.pos.y > 780.f)
				{
					std::cout << "swordsman fell to death" << std::endl;
					enemy.health = 0;
					continue;
				}

				// Player/enemy combat (your existing code)
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
							std::cout << "Enemy defeated!" << std::endl;
							m_skillTree.AddSkillPoint();
						}
					}
				}

				if (enemy.canDamagePlayer && !enemy.hasDealtDamage)
				{
					float dx = enemy.attackHitbox.getPosition().x - m_Player.pos.x;
					float dy = enemy.attackHitbox.getPosition().y - m_Player.pos.y;
					float distance = std::sqrt(dx * dx + dy * dy);

					if (m_Player.canBlockEnemy)
					{
						std::cout << "Attack blocked!" << std::endl;
						float knockbackDirection = (m_Player.pos.x > enemy.pos.x) ? 1.0f : -1.0f;
						m_Player.velocity.x = knockbackDirection * 200.f;
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

			// ========== UPDATE ARCHERS WITH COLLISION ==========
			for (auto& archer : m_archers)
			{
				if (archer.health <= 0) continue;

				// Store old position
				sf::Vector2f oldPos = archer.pos;

				// Update archer (AI + movement)
				archer.Update(dt, m_Player.pos);
				// ledge coverage 
				bool wouldFallOffLedge = EnemyCollision::IsLedgeAhead(
					oldPos,
					archer.facingRight,
					m_chunks,
					45.f
				);

				if (wouldFallOffLedge && archer.velocity.x != 0.f)
				{
					// ARCHERS: Back away from ledge (they prefer range anyway!)
					archer.pos.x = oldPos.x;

					// Back up to maintain distance
					float backupDir = archer.facingRight ? -1.f : 1.f;
					archer.velocity.x = backupDir * archer.speed * 0.3f;
					archer.pos.x += archer.velocity.x * dt;
				}


				// Apply horizontal collision
				EnemyCollision::CheckHorizontalCollision(
					archer.pos,
					archer.velocity,
					m_chunks,
					dt
				);

				// Snap to ground
				EnemyCollision::ApplyGravityAndGround(
					archer.pos,
					archer.velocity,  // Pass velocity so gravity can update it
					m_chunks,
					dt
				);

				// Update sprite position
				if (archer.sprite)
					archer.sprite->setPosition(archer.pos);

				if (archer.pos.y > 780)
				{
					std::cout << "archer fell to death" << std::endl;
					archer.health = 0;
					continue;
				}

				// Arrow spawning (your existing code)
				if (archer.state == Enemy2::ArcherState::Attacking
					&& archer.canDamagePlayer
					&& !archer.hasDealtDamage)
				{
					sf::Vector2f arrowStart = archer.pos;
					arrowStart.x += archer.facingRight ? 40.f : -40.f;
					m_arrows.emplace_back(arrowStart, archer.facingRight);
					archer.hasDealtDamage = true;
					std::cout << "Archer shot arrow!" << std::endl;
				}

				// Player attacking archer (your existing code)
				if (m_Player.canDamageEnemy)
				{
					float dx = m_Player.attackHitbox.getPosition().x - archer.pos.x;
					float dy = m_Player.attackHitbox.getPosition().y - archer.pos.y;
					float distance = std::sqrt(dx * dx + dy * dy);

					if (distance < m_Player.attackHitboxRadius + 30.f)
					{
						archer.TakeDamage(1);
						m_Player.canDamageEnemy = false;

						if (archer.health <= 0)
						{
							std::cout << "Archer defeated!" << std::endl;
							m_skillTree.AddSkillPoint();
						}
					}
				}
			}


			// ========== UPDATE ARROWS (SEPARATE LOOP!) ==========
			for (auto& arrow : m_arrows)
			{
				if (!arrow.active) continue;

				arrow.Update(dt);

				// Check if arrow hit player
				sf::FloatRect arrowBounds = arrow.getBounds();
				sf::FloatRect playerBounds(
					{ m_Player.pos.x - 15.f, m_Player.pos.y - 20.f },
					{ 30.f, 40.f }
				);

				if (arrowBounds.findIntersection(playerBounds).has_value())
				{
					if (m_Player.canBlockEnemy)
					{
						std::cout << "Arrow blocked!" << std::endl;
						arrow.active = false;
					}
					else
					{
						m_Player.TakeDamage(1);
						arrow.active = false;
						std::cout << "Arrow hit player! Health: " << m_Player.health << std::endl;
					}
				}

				// Remove arrows that are off-screen
				if (arrow.IsOffScreen(m_cameraOffset.x, m_window.getSize().x))
				{
					arrow.active = false;
				}
			}

			// Clean up inactive arrows
			m_arrows.erase(
				std::remove_if(m_arrows.begin(), m_arrows.end(),
					[](const Arrow& arrow) { return !arrow.active; }),
				m_arrows.end()
			);

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
void Game::render()
{
	m_window.clear(sf::Color(20, 20, 30));

	if (m_showMenu)
	{
		m_mainMenu->Draw(m_window);
	}
	else
	{
		// Apply camera view BEFORE rendering world
		m_window.setView(m_gameView);

		sf::Vector2f renderPos = m_Player.pos - m_cameraOffset;
		m_Player.sprite->setPosition(renderPos);

		// ===== HUB vs EXPEDITION =====
		if (m_isInHub)
		{
			// USE HUB CLASS TO RENDER
			m_hub.Render(m_window, m_chunks, m_Player, m_cameraOffset,
				m_showDebugCollision, PLAYER_HITBOX_WIDTH, PLAYER_HITBOX_HEIGHT);
		}
		else  // EXPEDITION MODE
		{
			// Render background
			m_dynamicBackground.render(m_window);

			// Render chunks
			for (auto& chunk : m_chunks)
				chunk.draw(m_window, m_cameraOffset);

			// Debug collision
			if (m_showDebugCollision)
			{
				Debug::DrawChunkCollision(m_window, m_chunks, m_cameraOffset);
				Debug::DrawPlayerCollision(m_window, m_Player, m_cameraOffset,
					PLAYER_HITBOX_WIDTH, PLAYER_HITBOX_HEIGHT);
				Debug::DrawAllEnemiesCollision(m_window, m_enemies, m_archers, m_cameraOffset);

			}

			// Render ENEMIES
			for (auto& enemy : m_enemies)
			{
				if (enemy.health <= 0) continue;
				if (!enemy.sprite) continue;

				sf::Vector2f enemyRenderPos = enemy.pos - m_cameraOffset;
				enemy.sprite->setPosition(enemyRenderPos);
				m_window.draw(*enemy.sprite);
			}

			// Render ARCHERS
			for (auto& archer : m_archers)
			{
				if (archer.health <= 0) continue;
				if (!archer.sprite) continue;

				sf::Vector2f archerRenderPos = archer.pos - m_cameraOffset;
				archer.sprite->setPosition(archerRenderPos);
				m_window.draw(*archer.sprite);
			}

			// Render ARROWS
			for (auto& arrow : m_arrows)
			{
				if (!arrow.active) continue;

				sf::Vector2f arrowRenderPos = arrow.getPosition() - m_cameraOffset;
				arrow.sprite.setPosition(arrowRenderPos);
				m_window.draw(arrow.sprite);
			}

			// Render player
			m_window.draw(*m_Player.sprite);

			// Render player UI
			for (auto& bar : m_Player.HealBar)
				m_window.draw(bar);
			for (int i = 0; i < m_Player.HealsCount; i++)
				m_window.draw(m_Player.HealSphere[i]);

			// Reset view for UI elements (BPM text, skill tree)
			m_window.setView(m_window.getDefaultView());

			// Draw BPM text
			m_window.draw(m_bpmText);

			for (int edge = 0; edge < 4; ++edge)
				for (int i = 0; i < BORDER_SLICES; ++i)
					m_window.draw(m_screenBorders[edge][i]);


			// Skill tree overlay
			if (m_showSkillTree)
			{
				sf::RectangleShape overlay(sf::Vector2f(m_window.getSize()));
				overlay.setFillColor(sf::Color(0, 0, 0, 180));
				m_window.draw(overlay);
				m_skillTree.Draw(m_window);
			}
		}
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
	m_bpmStream.setVolume(50.0f); // You had it at 0, probably want to hear it!
	m_bpmStream.play();

}


void Game::updateChunks()
{
	// Find rightmost chunk
	float rightmostX = -999999.0f;
	int rightmostIndex = 0;

	for (int i = 0; i < m_chunks.size(); ++i)
	{
		float chunkRight = m_chunks[i].getPosition().x + m_chunks[i].getWidth();
		if (chunkRight > rightmostX)
		{
			rightmostX = chunkRight;
			rightmostIndex = i;
		}
	}

	// If player is getting close to the right edge recyle leftmost chunk
	float rightEdge = rightmostX;
	if (m_Player.pos.x > rightEdge + 800 - (m_chunkWidth * 2))
	{
		// Find leftmost chunk
		float leftmostX = 999999.0f;
		int leftmostIndex = 0;

		for (int i = 0; i < m_chunks.size(); ++i)
		{
			if (m_chunks[i].getPosition().x < leftmostX)
			{
				leftmostX = m_chunks[i].getPosition().x;
				leftmostIndex = i;
			}
		}

		// Only recycle if leftmost is  behind playe
		if (m_Player.pos.x > leftmostX + m_chunkWidth)
		{
			float newX = rightmostX;
			loadChunkAt(leftmostIndex, newX);
		}
	}
}

bool Game::loadChunkAt(int index, float xPosition)
{
	std::vector<std::string> chunkPaths = DynamicBackground::GetChunkPaths(m_currentGameTheme);

	int randomIndex = rand() % chunkPaths.size();
	std::string chunkFile = chunkPaths[randomIndex];

	std::string tilesetPath = DynamicBackground::GetTilesetPath(m_currentGameTheme);

	if (!m_chunks[index].load(chunkFile, m_tilesetTexture, 32, tilesetPath))
	{
		std::cout << "Failed to load chunk at index " << index << std::endl;
		return false;
	}

	m_chunks[index].setPosition(sf::Vector2f(xPosition, 190.0f));
	std::cout << "Loaded chunk " << index << " at x: " << xPosition
		<< " (width: " << m_chunks[index].getWidth() << ")" << std::endl;
	return true;
}


