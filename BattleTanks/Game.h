#pragma once

#include "Defs.h"
#include "ResourceManager.h"
#include "Telegram.h"

#include <ctime>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

class EntityManager;
class SoundAsset;
class TextureAsset;
class MusicAsset;
class Map;

class Game
{
private:
	Game(void);
	Game(Game const &game); // hide copy constructor
	Game& operator=(Game const &game); // hide assignment operator

public:
	~Game(void);

	static Game& instance();
	TIME getTime() { return mGameTime.asSeconds(); };

	int start(); //load all assets
	int run();   //start playing
	int end();   //game is over

	Map& getMap();

	static ResourceManager<sf::Font*>& Font();
	static ResourceManager<SoundAsset*>& Sound();
	static ResourceManager<MusicAsset*>& Music();
	static ResourceManager<TextureAsset*>& Texture();

	bool handleMessage(Telegram const &telegram);

	int demoMode;

private:
	int mState;
	int mAICount;
	int mAIKillCount;
	TIME nextSpawnTime;
	bool mIsGameOver;
	//clock_t lastFrameTime, currentFrameTime, frameTime, initialTime;
	sf::Clock mClock;
	sf::Time mGameTime;

	sf::RenderWindow* window;

	Map* map;

	void processEvents();

	void startFrame();
	void endFrame();
};

