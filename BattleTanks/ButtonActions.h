/*

#pragma once

#include "MouseListener.h"
#include "Vec2.h"
#include <SFML/Window/Event.hpp>
#include "Bu

class ButtonMouseListener : public MouseListener
{
	friend class Button;
public:
	ButtonMouseListener(Vec2 absmin, Vec2 absmax, Button& self) :
		MouseListener(absmin, absmax, true), //use overloaded constructor to notify this listener it is for menus
		self(self)
	{};
		
	virtual ~ButtonMouseListener() {};

	virtual void mouseHover(sf::Event& e);
	virtual void mouseHoverOut(sf::Event& e);
	virtual void mousePressed(sf::Event& e);
	virtual void mouseClick(sf::Event& e);

private:
	Button& self;
};
*/