#include <SDL.h>

#include "Mouse.h"
#include "MouseListener.h"
#include "Vec2.h"

#include <iostream>

Mouse::Mouse() :
	leftButtonDown(false),
	rightButtonDown(false),
	leftButtonDownPos(Vec2(0, 0)),
	rightButtonDownPos(Vec2(0, 0))
{
}

Mouse& Mouse::instance()
{
	static Mouse instance;
	return instance;
}

void Mouse::process(sf::Event& e)
{
	if (e.type == sf::Event::MouseMoved)
		e.mouseMove.y = WINDOW_HEIGHT - e.mouseMove.y; //convert screen to world space
	else
		e.mouseButton.y = WINDOW_HEIGHT - e.mouseButton.y; //convert screen to world space

	//std::cout << e.mouseMove.x << ", " << e.mouseMove.y << std::endl;

	if (e.type == sf::Event::MouseButtonPressed) //mouse down event
	{
		if (e.mouseButton.button == 0)
		{
			leftButtonDown = true;
			leftButtonDownPos = Vec2((float)e.mouseButton.x, (float)e.mouseButton.y);
			notifyMousePressed(e);
		}
		else if (e.mouseButton.button == 1)
		{
			rightButtonDown = true;
			rightButtonDownPos = Vec2((float)e.mouseButton.x, (float)e.mouseButton.y);
		}
	}
	else if (e.type == sf::Event::MouseButtonReleased) //mouse up event
	{
		if (e.mouseButton.button == 0)
		{
			leftButtonDown = false;
			Vec2 curPos((float)e.mouseButton.x, (float)e.mouseButton.y);
			if ((curPos - leftButtonDownPos).vlen() < MAX_CLICK_RADIUS) //register a click if the release was close to the press position
				notifyMouseClick(e);
			else
				notifyMouseReleased(e);
		}
		else if (e.mouseButton.button == 1)
		{
			rightButtonDown = false;
			Vec2 curPos((float)e.mouseButton.x, (float)e.mouseButton.y);
			if ((curPos - rightButtonDownPos).vlen() < MAX_CLICK_RADIUS) //register a click if the release was close to the press position
				notifyMouseContextClick(e);
			else
				notifyMouseReleased(e);
		}
	}
	else if (e.type == sf::Event::MouseMoved) //mouse movement
	{
		if (leftButtonDown)
		{
			Vec2 curPos((float)e.mouseButton.x, (float)e.mouseButton.y);
			//mouse movement
			if (leftButtonDown && (curPos - leftButtonDownPos).vlen() >= MAX_CLICK_RADIUS) //left clicking + moved more than click radius -> this is a drag
				notifyMouseDrag(e);
		}
		else //no buttons down, just hovering
			notifyMouseHover(e);
	}
	else if (e.type == sf::Event::MouseWheelMoved)
	{
		if (e.mouseWheel.delta > 0)
			notifyMouseMWheelUp(e);
		else if (e.mouseWheel.delta < 0)
			notifyMouseMWheelDown(e);
	}
}

void Mouse::notifyMousePressed(sf::Event& e)
{
	for (auto it = listeners.begin(); it != listeners.end(); ++it)
	{
		if ((*it)->isPositionValid(e)) //do only if inside the boundaries of this component
			(*it)->mousePressed(e);
	}
}

void Mouse::notifyMouseReleased(sf::Event& e)
{
	for (auto it = listeners.begin(); it != listeners.end(); ++it)
	{
		if ((*it)->isPositionValid(e)) //do only if inside the boundaries of this component
			(*it)->mouseReleased(e);
	}
}

void Mouse::notifyMouseClick(sf::Event& e)
{
	for (auto it = listeners.begin(); it != listeners.end(); ++it)
	{
		if ((*it)->isPositionValid(e)) //do only if inside the boundaries of this component
			(*it)->mouseClick(e);
	}
}

void Mouse::notifyMouseContextClick(sf::Event& e)
{
	for (auto it = listeners.begin(); it != listeners.end(); ++it)
	{
		if ((*it)->isPositionValid(e)) //do only if inside the boundaries of this component
			(*it)->mouseContextClick(e);
	}
}

void Mouse::notifyMouseDrag(sf::Event& e)
{
	for (auto it = listeners.begin(); it != listeners.end(); ++it)
	{
		if ((*it)->isPositionValid(e)) //do only if inside the boundaries of this component
			(*it)->mouseDrag(e);
	}
}

void Mouse::notifyMouseHover(sf::Event& e)
{
	for (auto it = listeners.begin(); it != listeners.end(); ++it)
	{
		if ((*it)->isPositionValid(e)) //do only if inside the boundaries of this component
		{
			(*it)->setMouseHover();
			(*it)->mouseHover(e);
		}
	}
}

void Mouse::notifyMouseMWheelUp(sf::Event& e)
{
	for (auto it = listeners.begin(); it != listeners.end(); ++it)
	{
		if ((*it)->isPositionValid(e)) //do only if inside the boundaries of this component
			(*it)->mouseMWheelUp(e);
	}
}

void Mouse::notifyMouseMWheelDown(sf::Event& e)
{
	for (auto it = listeners.begin(); it != listeners.end(); ++it)
	{
		if ((*it)->isPositionValid(e)) //do only if inside the boundaries of this component
			(*it)->mouseMWheelDown(e);
	}
}