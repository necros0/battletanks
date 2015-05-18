#pragma once

#include "Vec2.h"

#include <SFML/Window.hpp>

#include <list>
#include <iostream>

class MouseListener;

class Mouse
{
private:
	Mouse();
	Mouse(Mouse const &mouse); // hide cop.y constructor
	Mouse& operator=(Mouse const &mouse); // hide assignment operator
public:
	~Mouse() {};
	void process(sf::Event& e);

	void addListener(MouseListener* listener) { listeners.push_back(listener); }
	void removeListener(MouseListener* listener) { listeners.remove(listener); }

	//maximum distance between press and release positions to determine either a mouse click or a mouse drag event
	static const int MAX_CLICK_RADIUS = 8;
	static Mouse& instance();

private:
	std::list<MouseListener*> listeners;
	bool leftButtonDown;
	Vec2 leftButtonDownPos;

	bool rightButtonDown;
	Vec2 rightButtonDownPos;

	void notifyMousePressed(sf::Event& e);
	void notifyMouseReleased(sf::Event& e);
	void notifyMouseClick(sf::Event& e);
	void notifyMouseContextClick(sf::Event& e);
	void notifyMouseDrag(sf::Event& e);
	void notifyMouseHover(sf::Event& e);
	void notifyMouseMWheelUp(sf::Event& e);
	void notifyMouseMWheelDown(sf::Event& e);
};

