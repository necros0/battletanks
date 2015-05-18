#pragma once

#include <SFML/Audio/Music.hpp>

#include <string>

class MusicAsset;

class MusicController
{
public:
	MusicController(void);
	~MusicController(void);

	static MusicController& instance();

	void play(MusicAsset*);

	float getVolume();
	void setVolume(float);

private:
	sf::Music mMusic;
	MusicAsset* mCurrentMusic; //in case we need to track what's playing..?
	float mVolume;
};

