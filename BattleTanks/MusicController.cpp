#include "MusicController.h"
#include "MusicAsset.h"

MusicController::MusicController(void) :
	mVolume(0.5f)
{
	this->mMusic.setLoop(true); //music always loops
	this->mMusic.setVolume(mVolume * 100);
}


MusicController::~MusicController(void)
{
}

MusicController& MusicController::instance()
{
	static MusicController instance;
	return instance;
}

void MusicController::play(MusicAsset* music)
{
	if (!music)
		return;

	this->mCurrentMusic = music;

	if (this->mMusic.getStatus() == sf::Music::Playing)
	{
		this->mMusic.stop();
	}
	
	this->mMusic.openFromFile(music->getPath());
	this->mMusic.play();
}

float MusicController::getVolume()
{
	return this->mVolume;
}

void MusicController::setVolume(float v)
{
	this->mVolume = v;
	this->mMusic.setVolume(v * 100);
}