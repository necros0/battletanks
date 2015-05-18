#include "Menu.h"
#include "Game.h"
#include "Vec2.h"

#include <iostream>

Menu::Menu(std::string title) :
	mTitle(title, *Game::Font().get("resources/font/Ubuntu-M.ttf"), 50)
{
	mTitle.setOrigin(Vec2(mTitle.getLocalBounds().width * 0.5f, mTitle.getLocalBounds().height * 0.5f));
	Vec2 labelOrigin = Vec2(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT - MENU_TOP);
	//labelOrigin.x = labelOrigin.x - (mTitle.getLocalBounds().width * 0.5f);
	//labelOrigin.y = labelOrigin.y + (mTitle.getLocalBounds().height * 0.5f);
	mTitle.setPosition(labelOrigin.toScreenVec());
}


Menu::~Menu(void)
{
	while(!mItems.empty())
	{
		delete(mItems.back());
		mItems.pop_back();
	}
}

void Menu::addItem(MenuItem* item)
{
	mItems.push_back(item);
}

float Menu::getNextItemHeight()
{
	return WINDOW_HEIGHT - (mItems.size() * 70) - MENU_TOP - 150.0f;
}

void Menu::draw(sf::RenderTarget& target)
{
	target.draw(mTitle);
	for (auto it = mItems.begin(); it != mItems.end(); it++)
	{
		(*it)->draw(target);
	}
}

void Menu::setActive(bool b)
{
	for (auto it = mItems.begin(); it != mItems.end(); it++)
	{
		(*it)->setActive(b);
	}
}
