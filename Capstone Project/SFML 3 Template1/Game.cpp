/// <summary>
/// author Pete Lowe May 2025
/// you need to change the above line or lose marks
/// </summary>
#include "Game.h"
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
	m_backgroundLayers.reserve(3);
	m_backgroundLayers.emplace_back("ASSETS/IMAGES/Autumn Forest 2D Pixel Art/Background/3.png", 1.0f);
	m_backgroundLayers.emplace_back("ASSETS/IMAGES/Autumn Forest 2D Pixel Art/Background/2.png", 0.8f);
	m_backgroundLayers.emplace_back("ASSETS/IMAGES/Autumn Forest 2D Pixel Art/Background/1.png", 0.5f);

	m_bpmAnalyzer = BPM(0.9f);
	if (!m_bpmAnalyzer.loadFile("ASSETS/AUDIO/your_song.wav"))
	{
		std::cerr << "Failed to load music file for BPM detection.\n";
	}

	m_Player.pos.x = m_window.getSize().x / 2.f;
	setupSprites(); // load texture
	setupTexts();   // load font
	m_Player.SetupPlayer();
	m_Player.HealCall();
	m_Player.Health();
	initNPCs();

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
	sf::Time timePerFrame = sf::seconds(1.0f / fps); // 60 fps
	while (m_window.isOpen())
	{
		processEvents(); // as many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // at least 60 fps
			update(timePerFrame); //60 fps
		}
		render(); // as many as possible
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
		if (newEvent->is<sf::Event::KeyPressed>()) //user pressed a key
		{
			processKeys(newEvent);
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


	sf::Vector2f direction{ 0.0f, 0.0f };


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


	float leftMargin = m_screenMargin;
	float rightMargin = m_window.getSize().x - m_screenMargin;

	float playerScreenX = m_Player.pos.x - m_cameraOffset.x;

	if (playerScreenX > rightMargin)
		m_cameraOffset.x += playerScreenX - rightMargin;
	else if (playerScreenX < leftMargin)
		m_cameraOffset.x -= leftMargin - playerScreenX;

	for (auto& layer : m_backgroundLayers)
	{
		layer.setOffset(m_cameraOffset);
	}

	float dt = t_deltaTime.asSeconds();
	m_Player.Update(dt);


	m_bpmAnalyzer.update();

	float intensity = m_bpmAnalyzer.getIntensityMultiplier();
	std::string bpmState = m_bpmAnalyzer.getBPMState();

	std::cout << "Current BPM: " << m_bpmAnalyzer.getBPM()
		<< "  State: " << bpmState << std::endl;

	if (m_DELETEexitGame)
	{
		m_window.close();
	}
}


/// <summary>
/// draw the frame and then switch buffers
/// </summary>
void Game::render()
{
	m_window.clear(sf::Color::Blue);

	sf::Vector2f renderPos = m_Player.pos - m_cameraOffset;
	m_Player.sprite->setPosition(renderPos);
	for (auto& layer : m_backgroundLayers)
	{
		layer.render(m_window);
	}

	for (int i = 0; i < m_Player.HealsCount; i++)
	{
		m_window.draw(m_Player.HealSphere[i]);
	}

	for (auto& bar : m_Player.HealBar)
	{
		m_window.draw(bar);
	}
	m_window.draw(*m_Player.sprite);
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

}

void Game::initNPCs()
{
	m_npcs.resize(4);

	for (size_t i = 0; i < m_npcs.size(); ++i)
	{
		m_npcs[i].SetupNpc();
	}

	m_npcs[0].pos = { 200.f, 100.f };
	m_npcs[1].pos = { 400.f, 200.f };
	m_npcs[2].pos = { 600.f, 200.f };
	m_npcs[3].pos = { 800.f, 100.f };
}
