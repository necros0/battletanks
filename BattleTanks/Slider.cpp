#include "Slider.h"
#include "Game.h"
#include "ChannelManager.h"
#include "MusicController.h"

#include <iostream>

const sf::Color Slider::COLOR_IDLE(0, 100, 0, 255);
const sf::Color Slider::COLOR_HOVER(0, 200, 0, 255);
const sf::Color Slider::COLOR_PRESSED(0, 255, 0, 255);
const sf::Color Slider::COLOR_HANDLE_IDLE(100, 190, 100, 255);
const sf::Color Slider::COLOR_HANDLE_HOVER(100, 255, 100, 255);
const sf::Color Slider::COLOR_HANDLE_PRESSED(255, 255, 255, 255);

#define SLIDER_HANDLE_WIDTH (5.0f)
#define SLIDER_HANDLE_HEIGHT (15.0f)

Slider::Slider(Vec2 mins, Vec2 size, std::string label, bool isMusic) :
	MenuItem(new SliderMouseListener(mins, mins + size, *this)),
	mBackgroundShape(size),

	mSliderWidth(size.x - 20.0f),
	mSliderBarPos(), //set later

	mSliderBarShape(Vec2(size.x - 20.0f, 7.0f)),
	mSliderHandleShape(Vec2(SLIDER_HANDLE_WIDTH, SLIDER_HANDLE_HEIGHT)),
	//mSliderHandleSize(Vec2(5.0f, 15.0f)),

	mLabel(label, *Game::Font().get("resources/font/Ubuntu-M.ttf"), 20),
	mSliderValue(ChannelManager::getManager().getVolume()),
	mIsMusicSlider(isMusic),	//HACK: cheap hack to get sound and music sliders
	mIsSliderHeld(false)
{
	//HACK
	if (mIsMusicSlider)
		mSliderValue = MusicController::instance().getVolume();

	mBackgroundShape.setFillColor(COLOR_IDLE);
	mBackgroundShape.setOrigin(size * 0.5f);
	Vec2 sOrg = mins;
	sOrg.x = sOrg.x + size.x * 0.5f;
	sOrg.y = sOrg.y + size.y * 0.5f;
	mBackgroundShape.setPosition(sOrg.toScreenVec());

	mSliderBarPos = sOrg - Vec2(0, 15.0f);


	mSliderBarShape.setOrigin(Vec2((size.x - 20.0f) * 0.5f, 5.0f * 0.5f));
	mSliderBarShape.setPosition(mSliderBarPos.toScreenVec()); //(sOrg - Vec2(0, 20.0f)).toScreenVec());
	mSliderBarShape.setFillColor(sf::Color(0, 255, 0, 255));

	mSliderHandleShape.setOrigin(Vec2(5.0f * 0.5f, 15.0f * 0.5f));
	mSliderHandleShape.setFillColor(COLOR_HANDLE_IDLE);
	setSliderHandleShapePosition();

	//Set label's origin to the middle of the text
	mLabel.setOrigin(Vec2(mLabel.getLocalBounds().width * 0.5f, mLabel.getLocalBounds().height * 0.5f + 20.0f));
	mLabel.setPosition(sOrg.toScreenVec());
}

void Slider::setSliderHandleShapePosition()
{
	mSliderHandleShape.setPosition(getSliderHandlePosition().toScreenVec());
}

Vec2 Slider::getSliderHandlePosition()
{
	return mSliderBarPos + Vec2((mSliderValue - 0.5f) * mSliderWidth, 0);
}

Slider::~Slider(void)
{
}

void Slider::draw(sf::RenderTarget& target)
{
	target.draw(mBackgroundShape);
	target.draw(mSliderBarShape);
	target.draw(mSliderHandleShape);
	target.draw(mLabel);
}

void Slider::SliderMouseListener::mouseHover(sf::Event& e)
{
	Vec2 handlePos = self.getSliderHandlePosition();
	if (e.mouseMove.x > handlePos.x - SLIDER_HANDLE_WIDTH * 0.8f  &&
		e.mouseMove.x < handlePos.x + SLIDER_HANDLE_WIDTH * 0.8f  &&
		e.mouseMove.y > handlePos.y - SLIDER_HANDLE_HEIGHT * 0.8f &&
		e.mouseMove.y < handlePos.y + SLIDER_HANDLE_HEIGHT * 0.8f)
		self.mSliderHandleShape.setFillColor(COLOR_HANDLE_HOVER);
	else
		self.mSliderHandleShape.setFillColor(COLOR_HANDLE_IDLE);
}

void Slider::SliderMouseListener::mouseHoverOut(sf::Event& e)
{
	self.mSliderHandleShape.setFillColor(COLOR_HANDLE_IDLE);
}

void Slider::SliderMouseListener::mousePressed(sf::Event& e)
{
	Vec2 handlePos = self.getSliderHandlePosition();
	if (e.mouseButton.x > handlePos.x - SLIDER_HANDLE_WIDTH * 0.8f  &&
		e.mouseButton.x < handlePos.x + SLIDER_HANDLE_WIDTH * 0.8f  &&
		e.mouseButton.y > handlePos.y - SLIDER_HANDLE_HEIGHT * 0.8f &&
		e.mouseButton.y < handlePos.y + SLIDER_HANDLE_HEIGHT * 0.8f)
	{
		//std::cout << "mousePressed" << std::endl;
		self.mSliderHandleShape.setFillColor(COLOR_HANDLE_PRESSED);
		self.mIsSliderHeld = true;
	}
}

void Slider::SliderMouseListener::mouseClick(sf::Event& e)
{
/*	Vec2 handlePos = self.getSliderHandlePosition();
	if (!(e.mouseMove.x > handlePos.x - SLIDER_HANDLE_WIDTH * 0.8f  &&
		e.mouseMove.x < handlePos.x + SLIDER_HANDLE_WIDTH * 0.8f  &&
		e.mouseMove.y > handlePos.y - SLIDER_HANDLE_HEIGHT * 0.8f &&
		e.mouseMove.y < handlePos.y + SLIDER_HANDLE_HEIGHT * 0.8f))
	{*/
		self.mSliderHandleShape.setFillColor(COLOR_HANDLE_IDLE);
		self.mIsSliderHeld = false;
	//}
}

void Slider::SliderMouseListener::mouseDrag(sf::Event& e)
{
	if (self.mIsSliderHeld)
	{
		self.mSliderValue = ( (e.mouseMove.x - self.mSliderBarPos.x) + (self.mSliderWidth * 0.5f) ) / self.mSliderWidth;
		if (self.mSliderValue < 0)
			self.mSliderValue = 0.0f;
		else if (self.mSliderValue > 1)
			self.mSliderValue = 1.0f;
		self.setSliderHandleShapePosition();

		if (self.mIsMusicSlider)
			MusicController::instance().setVolume(self.mSliderValue);
		else
			ChannelManager::getManager().setVolume(self.mSliderValue);
		//std::cout << self.mSliderValue << std::endl;
	}
}