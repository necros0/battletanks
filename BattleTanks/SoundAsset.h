#pragma once

#include <SFML/Audio.hpp>

#include <string>
#include <list>

class SoundAsset
{
public:
	SoundAsset(std::string path);
	~SoundAsset(void);

	void play(float vol, int chan);
	

private:
	sf::SoundBuffer* mSoundBuffer;	//the buffer to play this sound on
	std::vector<char> mSound;		//sound data stored in memory
};

