#include "SoundAsset.h"
#include "ChannelManager.h"

#include <iostream>
#include <fstream>

SoundAsset::SoundAsset(std::string path)
{
	std::ifstream file(path, std::ios::binary);
	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	mSound = std::vector<char>(size);
	if (!file.read(mSound.data(), size))
	{
		std::cerr << "Couldn't load file into memory: " << path << std::endl;
		return;
	}

	this->mSoundBuffer = new sf::SoundBuffer();
	if (!this->mSoundBuffer->loadFromMemory(mSound.data(), size))
	{
		std::cerr << "Couldn't link file in memory to buffer: " << path << std::endl;
		return;
	}
}


SoundAsset::~SoundAsset(void)
{
	delete(this->mSoundBuffer); //delete the buffer
}

void SoundAsset::play(float vol, int chan)
{
	sf::Sound* channel = ChannelManager::getManager().getChannel(vol * 100, chan);
	channel->setBuffer(*mSoundBuffer);
	channel->play();
}

