#pragma once

#include "Vec2.h"

#include <SFML/Window.hpp>

class MouseListener
{
public:
	MouseListener(Vec2 mouseAbsMins, Vec2 mouseAbsMaxs);
	MouseListener(Vec2 mouseAbsMins, Vec2 mouseAbsMaxs, bool);

	virtual ~MouseListener();
	virtual void mousePressed(sf::Event& e) {};
	virtual void mouseReleased(sf::Event& e) {};
	virtual void mouseClick(sf::Event& e) {};
	virtual void mouseContextClick(sf::Event& e) {};
	virtual void mouseDrag(sf::Event& e) {};
	virtual void mouseHover(sf::Event& e) {};
	virtual void mouseHoverOut(sf::Event& e) {};
	virtual void mouseMWheelUp(sf::Event& e) {};
	virtual void mouseMWheelDown(sf::Event& e) {};

	void setMouseHover();
	bool isPositionValid(sf::Event& e);

private:
	Vec2 mAbsMins;
	Vec2 mAbsMaxs;
	bool mIsMenuListener;
	bool mIsMouseHover;
};
