#include "Button.h"
#include "Game.h"
#include "IBtnCmd.h"

const sf::Color Button::COLOR_IDLE(0, 100, 0, 255);
const sf::Color Button::COLOR_HOVER(0, 200, 0, 255);
const sf::Color Button::COLOR_PRESSED(0, 255, 0, 255);

Button::Button(Vec2 mins, Vec2 size, std::string label, IBtnCmd* cmd) :
	MenuItem(new ButtonMouseListener(mins, mins + size, *this)),
	shape(size),
	mLabel(label, *Game::Font().get("resources/font/Ubuntu-M.ttf"), 20),
	mCommand(cmd)
{
	shape.setFillColor(COLOR_IDLE);
	shape.setOrigin(size * 0.5f);
	Vec2 sOrg = mins;
	sOrg.x = sOrg.x + size.x * 0.5f;
	sOrg.y = sOrg.y + size.y * 0.5f;
	shape.setPosition(sOrg.toScreenVec());

	//Set label's origin to the middle of the text
	mLabel.setOrigin(Vec2(mLabel.getLocalBounds().width * 0.5f, mLabel.getLocalBounds().height * 0.5f));
	mLabel.setPosition(sOrg.toScreenVec());
}


Button::~Button(void)
{
	if (mCommand)
	{
		delete(mCommand);
		mCommand = NULL;
	}
}

void Button::draw(sf::RenderTarget& target)
{
	target.draw(shape);
	target.draw(mLabel);
}

void Button::ButtonMouseListener::mouseHover(sf::Event& e)
{
	self.shape.setFillColor(COLOR_HOVER);
}

void Button::ButtonMouseListener::mouseHoverOut(sf::Event& e)
{
	self.shape.setFillColor(COLOR_IDLE);
}

void Button::ButtonMouseListener::mousePressed(sf::Event& e)
{
	self.shape.setFillColor(COLOR_PRESSED);
}

void Button::ButtonMouseListener::mouseClick(sf::Event& e)
{
	self.shape.setFillColor(COLOR_HOVER);
	if (self.mIsActive && self.mCommand)
		self.mCommand->run();
}