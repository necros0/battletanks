#pragma once

#include "MenuItem.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

#include <string>
#include <list>

#define MENU_TOP (120.0f)

class Menu
{
public:
	Menu(std::string);
	virtual ~Menu(void);

	void draw(sf::RenderTarget& target);
	void addItem(MenuItem*);
	float getNextItemHeight();

	void setActive(bool b);

private:
	sf::Text mTitle;
	std::list<MenuItem*> mItems;

};

