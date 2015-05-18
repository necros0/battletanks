#pragma once

#include <SFML/Audio/Sound.hpp>

#include <list>

class ChannelManager
{
public:
	ChannelManager(void);
	~ChannelManager(void);

	static ChannelManager& getManager();

	sf::Sound* getChannel(float vol, int chan);
	std::list<sf::Sound*> anonymousChannels;

	void setVolume(float v);
	float getVolume() { return mSoundVolume; };

private:
	float mSoundVolume;
};

