#include "Defs.h"
#include "Utility.h"
#include "Game.h"
#include "Map.h"
#include "Sector.h"
#include "InputManager.h"
#include "Tank.h"
#include "EntityManager.h"
#include "DebugDraw.h"
#include "Mouse.h"
#include "ResourceLoader.h"
#include "GUIController.h"
#include "MessageDispatcher.h"
#include "MusicController.h"
#include "MessageDefs.h"
#include "CollisionSolver.h"
#include "Powerup.h"

#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>

Game::Game(void) :
	window(NULL),
	mState(0),
	mAICount(0),
	mAIKillCount(0),
	mIsGameOver(false),
	demoMode(0),
	nextSpawnTime(0)
{
}


Game::~Game(void)
{
	if (window)
	{
		delete(window);
		window = NULL;
	}

	if (this->map)
	{
		delete(this->map);
		this->map = NULL;
	}

	//EntityManager::instance().clear();
	//Sound().clear();
	//Music().clear();
}

Game& Game::instance()
{
	static Game instance;
	return instance;
}

int Game::start()
{
	//Load resources used by the game
	ResourceLoader l; 
	l.init();

	//anti aliasing
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	// Create the main window
	window = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Battle Tanks", sf::Style::Default, settings);
	window->setFramerateLimit(50);
	window->setVerticalSyncEnabled(true);
	window->setVisible(true);

	//Link input manager stuffs
	InputManager::instance().registerKeymapping(sf::Keyboard::W, true, MSG_P_MOVE_UP, 1);
	InputManager::instance().registerKeymapping(sf::Keyboard::A, true, MSG_P_MOVE_LEFT, 1);
	InputManager::instance().registerKeymapping(sf::Keyboard::S, true, MSG_P_MOVE_DOWN, 1);
	InputManager::instance().registerKeymapping(sf::Keyboard::D, true, MSG_P_MOVE_RIGHT, 1);

	InputManager::instance().registerKeymapping(sf::Keyboard::W, false, MSG_M_MOVE_UP, 1);
	InputManager::instance().registerKeymapping(sf::Keyboard::A, false, MSG_M_MOVE_LEFT, 1);
	InputManager::instance().registerKeymapping(sf::Keyboard::S, false, MSG_M_MOVE_DOWN, 1);
	InputManager::instance().registerKeymapping(sf::Keyboard::D, false, MSG_M_MOVE_RIGHT, 1);

	//Menu key mappings
	InputManager::instance().registerKeymapping(sf::Keyboard::Escape, false, MSG_MENU_ESC, -1);
	InputManager::instance().registerKeymapping(sf::Keyboard::F4, false, MSG_MENU_ESC, -1);
	InputManager::instance().registerKeymapping(sf::Keyboard::F3, false, MSG_MENU_LOAD, -1);
	InputManager::instance().registerKeymapping(sf::Keyboard::F2, false, MSG_MENU_SAVE, -1);

	return EXIT_SUCCESS;
}

int Game::run()
{
	MessageDispatcher::instance().dispatchMessage(0, -2, -1, MessageType::MSG_MENU_START, NULL);

	MusicController::instance().play(Music().get("resources/music/intro_loop.ogg"));

	mClock.restart();
	sf::Time physTime; //time for physics to run, start at 0
	sf::Time physDelta = sf::seconds(0.016667f); //run physics at 60fps
	sf::Time frametime;
	sf::Time lastFrameTime;

	/* ---------------------- INITIAL MENU LOOP ---------------------- */
	while (mState == 0)
	{
		int frameskip = 0; //reset frameskip counter
		this->mGameTime = mClock.getElapsedTime(); //current time for this frame

		// Process events
		sf::Event event;
		while (window->pollEvent(event))
		{
			switch(event.type)
			{
			case sf::Event::Closed: // Close window : exit
				mState = 2;
				break;
			case sf::Event::MouseButtonPressed:
			case sf::Event::MouseButtonReleased:
			case sf::Event::MouseMoved:
				Mouse::instance().process(event);
				break;
			}
		}
		// Clear screen
		window->clear();

		if (GUIController::instance().isMenuActive())
			GUIController::instance().draw(*window);
		
		// Update the window
		window->display();

		//needed for GUI controls.  We can destroy gui components while listeners are still looping, so do message dispatching after
		MessageDispatcher::instance().justDispatch();

		frametime = mGameTime - lastFrameTime;
		lastFrameTime = mGameTime;

	}
	/* ---------------------- INITIAL MENU LOOP ---------------------- */

	//#################################
	//mState = 1; //############################################################################################################
	//#################################

	if (mState != 1) //already not playing (eg: chose to quit in the previous menu)
		return EXIT_SUCCESS;


	//Start the music for the level
	MusicController::instance().play(Music().get("resources/music/electron.ogg"));

	this->map = new Map(); //generate the map


	////////////////////////
	//Spawn the player
	Tank* tank;// = EntityManager::instance().getEntityByID(1);
	//if (!tank) //if player hasn't been spawned yet
		tank = new Tank(true); //create a new player
	EntityManager::instance().spawn(tank);
	GUIController::instance().setMonitoring(tank);
	
	if (Game::instance().demoMode == 1)
		this->map->spawnAI();
	else
		nextSpawnTime = 5.0f; //spawn 5 seconds after map load

	//EntityManager::instance().spawn(new PowerupKinetic(Vec2(500.0f, 350.0f)));
	EntityManager::instance().spawn(new PowerupKinetic(map->getRandomSector()->getCenter()));
	EntityManager::instance().spawn(new PowerupShield(map->getRandomSector()->getCenter()));

	EntityManager::instance().flushSpawns(); //do all spawns that were queued during init
	mClock.restart(); //restart clock for level start
	while (mState == 1) // Start the game loop
	{
		int frameskip = 0; //reset frameskip counter
		this->mGameTime = mClock.getElapsedTime(); //current time for this frame

		//Clear debug graphics pushed to the collection
		if (Game::instance().demoMode == 0)
			DebugDraw::clear();

		// Process events
		processEvents();

		// Clear screen
		window->clear();

		////////////////////////////////////////
		this->map->draw(*window); //Draw the map.
		////////////////////////////////////////

		if (mAICount < 20 && getTime() > nextSpawnTime && Game::instance().demoMode == 0)
		{
			nextSpawnTime = getTime() + 10.0f - (mAICount * 0.3f) - random() * 4.0f;
			this->map->spawnAI();
			++mAICount;
		}

		EntityManager::instance().update(frametime.asSeconds());

		while (this->mGameTime > physTime && frameskip < 6)
		{
			CollisionSolver::evaluate(physDelta.asSeconds()); //run physics until we've caught up
			physTime = physTime + physDelta;
			++frameskip;
		}
		EntityManager::instance().updateFromPhysics(); //update positions of entities from physics engine
		EntityManager::instance().draw(*window);

		//Draw all the debug stuff (lines, text)
		DebugDraw::display(*window);

		GUIController::instance().draw(*window);
		
		// Update the window
		window->display();

		MessageDispatcher::instance().justDispatch(); //take care of queued messages

		if (mIsGameOver)
			mState = 3;

		frametime = mGameTime - lastFrameTime;
		lastFrameTime = mGameTime;
	}

	return EXIT_SUCCESS;
}

int Game::end()
{
	EntityManager::instance().clear(); //clear entities

	if (mState != 3)
		return EXIT_SUCCESS;

	//victory/defeat
	if (mAIKillCount >= 20)
		MessageDispatcher::instance().dispatchMessage(0, -2, -1, MessageType::MSG_MENU_WIN, NULL);
	else
		MessageDispatcher::instance().dispatchMessage(0, -2, -1, MessageType::MSG_MENU_LOSS, NULL);

	while (mState == 3)
	{
		startFrame();

		// Process events
		sf::Event event;
		while (window->pollEvent(event))
		{
			switch(event.type)
			{
			case sf::Event::Closed: // Close window : exit
				mState = 2;
				break;
			case sf::Event::MouseButtonPressed:
			case sf::Event::MouseButtonReleased:
			case sf::Event::MouseMoved:
				Mouse::instance().process(event);
				break;
			}
		}
		// Clear screen
		window->clear();

		if (GUIController::instance().isMenuActive())
			GUIController::instance().draw(*window);
		
		// Update the window
		window->display();

		//needed for GUI controls.  We can destroy gui components while listeners are still looping, so do message dispatching after
		MessageDispatcher& m = MessageDispatcher::instance();
		MessageDispatcher::instance().justDispatch();

		endFrame();
	}

	return EXIT_SUCCESS;
}

Map& Game::getMap()
{
	return *this->map;
}

ResourceManager<MusicAsset*>& Game::Music()
{
	static ResourceManager<MusicAsset*> instance_music;
	return instance_music;
}
ResourceManager<SoundAsset*>& Game::Sound()
{
	static ResourceManager<SoundAsset*> instance_sound;
	return instance_sound;
}

ResourceManager<sf::Font*>& Game::Font()
{
	static ResourceManager<sf::Font*> instance_font;
	return instance_font;
}

ResourceManager<TextureAsset*>& Game::Texture()
{
	static ResourceManager<TextureAsset*> instance_texture;
	return instance_texture;
}

bool Game::handleMessage(Telegram const &telegram)
{
	if (telegram.msg == MessageType::MSG_GAME_START && mState == 0)
	{
		mState = 1;
	}
	else if (telegram.msg == MessageType::MSG_GAME_QUIT)
	{
		mState = 2;
	}
	//else if (telegram.msg == MessageType::MSG_AI_SPAWNED)
	//	++mAICount;
	else if (telegram.msg == MessageType::MSG_AI_DEATH)
	{
		++mAIKillCount; //increment kill counter
		if (mAIKillCount == 20)
		{
			GUIController::instance().setMonitoring(NULL);
			mIsGameOver = true;
			return true;
		}
		/*
		if (mAICount < 20) //if any left to spawn
		{
			map->spawnAI(); //spawn
			++mAICount;
		}
		
		//The higher the kill count, the more chance of spawning doubles.  after 17, guaranteed chance.
		if (mAICount < 20 && (mAICount > mAIKillCount) && random() < (mAIKillCount / 17.0f))
		{
			map->spawnAI(); //spawn
			++mAICount;
		}
		*/
	}
	else if (telegram.msg == MessageType::MSG_PLAYER_DEATH)
	{
		mIsGameOver = true;
	}
	else if (telegram.msg == MSG_POWERUP_KINETIC_RESPAWN)
	{
		EntityManager::instance().spawn(new PowerupKinetic(map->getRandomSector()->getCenter()));
	}
	else if (telegram.msg == MSG_POWERUP_SHIELD_RESPAWN)
	{
		EntityManager::instance().spawn(new PowerupShield(map->getRandomSector()->getCenter()));
	}

	return true;
}

void Game::processEvents()
{
	sf::Event event;
	while (window->pollEvent(event))
	{
		switch(event.type)
		{
		case sf::Event::Closed: // Close window : exit
			mState = 2;
			break;
		case sf::Event::KeyPressed:
			InputManager::instance().handleEvent(event.key.code, true);
			break;
		case sf::Event::KeyReleased:
			InputManager::instance().handleEvent(event.key.code, false);
			break;
		case sf::Event::MouseButtonPressed:
		case sf::Event::MouseButtonReleased:
		case sf::Event::MouseMoved:
			Mouse::instance().process(event);
			break;
		}
	}
}

void Game::startFrame()
{
	//this->currentFrameTime = clock();
	//this->gameTime = 
}

void Game::endFrame()
{
	//this->frameTime = (this->currentFrameTime - this->lastFrameTime); //get delta time (how long it took to process this frame)
	//this->lastFrameTime = this->currentFrameTime; //store old time
//gameTime = (this->currentFrameTime - this->initialTime) / 1000.0f;
	//gameTime = gameTime + (this->frameTime / 1000.0f);
}
