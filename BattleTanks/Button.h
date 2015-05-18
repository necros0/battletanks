#pragma once

#include "MenuItem.h"
#include "MouseListener.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Color.hpp>

#include <string>
#include <iostream>

class IBtnCmd;

class Button : public MenuItem
{
public:
	Button(Vec2, Vec2, std::string, IBtnCmd*);
	virtual ~Button(void);

	//Draw yourself
	void draw(sf::RenderTarget& target);

private:
	sf::RectangleShape shape;
	sf::Text mLabel;
	IBtnCmd* mCommand;

	static const sf::Color COLOR_HOVER;
	static const sf::Color COLOR_IDLE;
	static const sf::Color COLOR_PRESSED;



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
};

