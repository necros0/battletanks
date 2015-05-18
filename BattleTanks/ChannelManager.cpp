#include "ChannelManager.h"


#include <stdexcept>

ChannelManager::ChannelManager(void) :
	mSoundVolume(1.0f)
{
}


ChannelManager::~ChannelManager(void)
{
	for (auto it = anonymousChannels.begin(); it != anonymousChannels.end(); it++)
	{
		delete(*it); //free up the memory
	}
	anonymousChannels.clear();
}

ChannelManager& ChannelManager::getManager()
{
	static ChannelManager instance;
	return instance;
}

sf::Sound* ChannelManager::getChannel(float vol, int chan)
{
	vol = vol * mSoundVolume;
	//Static channels allow sounds to be interrupted
	static sf::Sound channel1; //menu channel
	static sf::Sound channel2;
	static sf::Sound channel3;
	static sf::Sound channel4;
	
	//first clean up any anonymous channels that have finished playing.
	for (auto it = anonymousChannels.begin(); it != anonymousChannels.end(); it++)
	{
		if ((*it)->getStatus() == sf::Sound::Playing)
			continue;

		delete(*it); //free up the memory
		it = anonymousChannels.erase(it); //get it out of the list
		if (it == anonymousChannels.end())
			break;
	}

	switch (chan)
	{
	case 1:
		channel1.setVolume(vol);
		return &channel1;
	case 2:
		channel2.setVolume(vol);
		return &channel2;
	case 3:
		channel3.setVolume(vol);
		return &channel3;
	case 4:
		channel4.setVolume(vol);
		return &channel4;
	default:
		anonymousChannels.push_back(new sf::Sound());
		anonymousChannels.back()->setVolume(vol);
		return anonymousChannels.back();
	}

	throw std::runtime_error("Switch for channel selection failed??!");
}

void ChannelManager::setVolume(float v)
{
	//Clamp values to legal ones
	if (v > 1.0f)
		v = 1.0f;
	else if (v < 0.0f)
		v = 0.0f;

	mSoundVolume = v;
}