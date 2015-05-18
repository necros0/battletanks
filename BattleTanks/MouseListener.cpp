#include "MouseListener.h"
#include "Mouse.h"
#include "GUIController.h"

#include <iostream>

MouseListener::MouseListener(Vec2 mouseAbsMins, Vec2 mouseAbsMaxs) : 
	mAbsMins(mouseAbsMins), mAbsMaxs(mouseAbsMaxs),
	mIsMenuListener(false),
	mIsMouseHover(false)
{
	Mouse::instance().addListener(this);
}

MouseListener::MouseListener(Vec2 mouseAbsMins, Vec2 mouseAbsMaxs, bool isMenu) : 
	mAbsMins(mouseAbsMins), mAbsMaxs(mouseAbsMaxs),
	mIsMenuListener(isMenu),
	mIsMouseHover(false)
{
	Mouse::instance().addListener(this);
}

MouseListener::~MouseListener()
{
	Mouse::instance().removeListener(this);
}

bool MouseListener::isPositionValid(sf::Event& e)
{
	//click is outside of element's boundaries
	if(    (e.type == sf::Event::MouseButtonPressed || e.type == sf::Event::MouseButtonReleased)
				&& (   e.mouseButton.x < mAbsMins.x || e.mouseButton.y < mAbsMins.y 
				    || e.mouseButton.x > mAbsMaxs.x || e.mouseButton.y > mAbsMaxs.y)
		 )
		return false;

	//click is outside of element's boundaries
	if (   e.type == sf::Event::MouseMoved
				&& (   e.mouseMove.x < mAbsMins.x || e.mouseMove.y < mAbsMins.y 
				    || e.mouseMove.x > mAbsMaxs.x || e.mouseMove.y > mAbsMaxs.y)
	    )
	{
		if (mIsMouseHover) //was hovering
		{
			mouseHoverOut(e); //send hover off
			mIsMouseHover = false; //mark this off
		}
		return false;
	}

	//If menu is on, but not a menu listener, ignore
	if (GUIController::instance().isMenuActive() && !mIsMenuListener)
		return false;


	return true;
}

void MouseListener::setMouseHover()
{
	mIsMouseHover = true;
}
